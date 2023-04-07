#include "book_storage_manager.hpp"
#include <vector>

using namespace domain::entities;

namespace application::utility
{

BookStorageManager::BookStorageManager(
    IBookStorageGateway* bookStorageGateway,
    IDownloadedBooksTracker* downloadedBooksTracker) :
    m_bookStorageGateway(bookStorageGateway),
    m_downloadedBooksTracker(downloadedBooksTracker)
{
    connect(m_bookStorageGateway,
            &IBookStorageGateway::gettingBooksMetaDataFinished, this,
            [this](const std::vector<Book>& books)
            {
                // Avoid storing books for logged out users by verifying login
                // status before adding books, else books might get loaded into
                // memory, even though the user is logged out.
                if(!userLoggedIn())
                    return;

                emit loadingRemoteBooksFinished(books);
            });

    connect(m_bookStorageGateway, &IBookStorageGateway::downloadingBookFinished,
            this, &BookStorageManager::saveDownloadedBookToFile);
}

void BookStorageManager::setUserData(const QString& email,
                                     const QString& authToken)
{
    m_authenticationToken = authToken;
    m_downloadedBooksTracker->setLibraryOwner(email);
}

void BookStorageManager::clearUserData()
{
    m_authenticationToken.clear();
    m_downloadedBooksTracker->clearLibraryOwner();
}

void BookStorageManager::saveDownloadedBookToFile(const QByteArray& data,
                                                  const QUuid& uuid)
{
    auto destDir = m_downloadedBooksTracker->getLibraryDir();
    QString fileName = uuid.toString(QUuid::WithoutBraces) + ".pdf";
    auto destination = QUrl(destDir.filePath(fileName)).path();

    QFile file(destination);
    if(!file.open(QIODevice::WriteOnly))
    {
        qDebug() << "Could not open test file!";
        return;
    }

    file.write(data);
    emit finishedDownloadingBook(uuid, destination);
}

bool BookStorageManager::userLoggedIn()
{
    return !m_authenticationToken.isEmpty();
}

QString BookStorageManager::getBookCoverPath(const QUuid& uuid)
{
    auto dir = QDir(m_downloadedBooksTracker->getLibraryDir());
    auto fileName = QString("%1%2.%3").arg(m_bookCoverPrefix,
                                           uuid.toString(QUuid::WithoutBraces),
                                           m_bookCoverType);

    return dir.filePath(fileName);
}

void BookStorageManager::addBook(const Book& bookToAdd)
{
    // Prevent adding remote books to the local library unless "downloaded" is
    // set to true.
    if(bookToAdd.getDownloaded())
        addBookLocally(bookToAdd);

    m_bookStorageGateway->createBook(m_authenticationToken, bookToAdd);
}

void BookStorageManager::addBookLocally(const domain::entities::Book& bookToAdd)
{
    m_downloadedBooksTracker->trackBook(bookToAdd);
}

void BookStorageManager::deleteBook(utility::BookForDeletion bookToDelete)
{
    // Remote books aren't in the local library, thus can't be untracked
    if(bookToDelete.downloaded)
        deleteBookLocally(bookToDelete.uuid);

    m_bookStorageGateway->deleteBook(m_authenticationToken, bookToDelete.uuid);
}

void BookStorageManager::deleteBookLocally(QUuid uuid)
{
    m_downloadedBooksTracker->untrackBook(uuid);
}

void BookStorageManager::uninstallBook(const QUuid& uuid)
{
    m_downloadedBooksTracker->untrackBook(uuid);
}

void BookStorageManager::downloadBook(const QUuid& uuid)
{
    m_bookStorageGateway->downloadBook(m_authenticationToken, uuid);
}

void BookStorageManager::updateBook(const Book& book)
{
    updateBookLocally(book);
    updateBookRemotely(book);
}

void BookStorageManager::updateBookLocally(const domain::entities::Book& book)
{
    // Prevent updating remote books in the local library unless the book is
    // downloaded. If its not downloaded, there is no local file to update.
    if(book.getDownloaded())
        m_downloadedBooksTracker->updateTrackedBook(book);
}

void BookStorageManager::updateBookRemotely(const domain::entities::Book& book)
{
    m_bookStorageGateway->updateBook(m_authenticationToken, book);
}

void BookStorageManager::changeBookCover(const Book& book)
{
    updateBook(book);

    if(book.hasCover())
    {
        auto pathToCover = getBookCoverPath(book.getUuid());
        m_bookStorageGateway->changeBookCover(m_authenticationToken,
                                              book.getUuid(), pathToCover);
    }
    else
    {
        m_bookStorageGateway->deleteBookCover(m_authenticationToken,
                                              book.getUuid());
    }
}

std::optional<QString> BookStorageManager::saveBookCoverToFile(
    const QUuid& uuid, const QPixmap& cover)
{
    QFile file(getBookCoverPath(uuid));
    if(!file.open(QFile::WriteOnly))
    {
        return std::nullopt;
    }

    int fileQuality = 20;
    cover.save(&file, m_bookCoverType.toStdString().c_str(), fileQuality);
    return file.fileName();
}

bool BookStorageManager::deleteBookCover(const QUuid& uuid)
{
    QFile file(getBookCoverPath(uuid));
    auto success = file.remove();

    return success;
}

std::vector<Book> BookStorageManager::loadLocalBooks()
{
    auto m_localBooks = m_downloadedBooksTracker->getTrackedBooks();
    return m_localBooks;
}

void BookStorageManager::loadRemoteBooks()
{
    m_bookStorageGateway->getBooksMetaData(m_authenticationToken);
}

}  // namespace application::utility