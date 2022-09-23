#pragma once
#include <QObject>
#include <QString>
#include <QList>
#include <QVariantMap>
#include "book_dto.hpp"


namespace adapters
{

class IBookController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(adapters::dtos::BookDto currentBook READ getCurrentBook NOTIFY currentBookChanged)
    Q_PROPERTY(int bookCount READ getBookCount NOTIFY bookCountChanged)
    
public:
    virtual ~IBookController() noexcept = default;
    
    
    Q_INVOKABLE virtual int addBook(const QString& path) = 0;
    Q_INVOKABLE virtual int deleteBook(const QString& title) = 0;
    Q_INVOKABLE virtual int updateBook(const QString& title,
                                       const QVariantMap& operations) = 0;
    Q_INVOKABLE virtual int addTag(const QString& title,
                                   const dtos::TagDto& tag) = 0;
    Q_INVOKABLE virtual int removeTag(const QString& title,
                                      const QString& tagName) = 0;
    Q_INVOKABLE virtual const dtos::BookDto* getBook(const QString& title) = 0;
    Q_INVOKABLE virtual int getBookCount() const = 0;
    Q_INVOKABLE virtual int setCurrentBook(QString title) = 0;
    Q_INVOKABLE virtual dtos::BookDto getCurrentBook() = 0;
    
signals:
    void currentBookChanged();
    void bookCountChanged();
};

} // namespace adapters