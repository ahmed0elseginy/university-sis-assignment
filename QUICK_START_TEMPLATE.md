# Quick Start Template

A minimal, ready-to-use module template you can copy and adapt.

## Complete Minimal Module Example

### 1. Data Model

```cpp
// modules/product/product.h
#ifndef PRODUCT_H
#define PRODUCT_H

#include <QString>

struct Product {
    int id = -1;
    QString name;
    double price = 0.0;
    int quantity = 0;
};

#endif // PRODUCT_H
```

### 2. Repository

```cpp
// modules/product/productrepository.h
#ifndef PRODUCTREPOSITORY_H
#define PRODUCTREPOSITORY_H

#include "product.h"
#include "database/databasemanager.h"
#include <vector>
#include <optional>

class ProductRepository {
public:
    bool addProduct(const Product& product);
    bool updateProduct(const Product& product);
    bool deleteProduct(int id);
    std::optional<Product> getProductById(int id);
    std::vector<Product> getAllProducts();
};

#endif // PRODUCTREPOSITORY_H
```

```cpp
// modules/product/productrepository.cpp
#include "productrepository.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

bool ProductRepository::addProduct(const Product& product) {
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("INSERT INTO products (name, price, quantity) VALUES (:name, :price, :quantity)");
    query.bindValue(":name", product.name);
    query.bindValue(":price", product.price);
    query.bindValue(":quantity", product.quantity);
    
    if (!query.exec()) {
        qDebug() << "Error:" << query.lastError().text();
        return false;
    }
    return true;
}

bool ProductRepository::updateProduct(const Product& product) {
    if (product.id < 0) return false;
    
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("UPDATE products SET name = :name, price = :price, quantity = :quantity WHERE product_id = :id");
    query.bindValue(":name", product.name);
    query.bindValue(":price", product.price);
    query.bindValue(":quantity", product.quantity);
    query.bindValue(":id", product.id);
    
    return query.exec();
}

bool ProductRepository::deleteProduct(int id) {
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("DELETE FROM products WHERE product_id = :id");
    query.bindValue(":id", id);
    return query.exec();
}

std::optional<Product> ProductRepository::getProductById(int id) {
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("SELECT product_id, name, price, quantity FROM products WHERE product_id = :id");
    query.bindValue(":id", id);
    
    if (query.exec() && query.next()) {
        Product product;
        product.id = query.value(0).toInt();
        product.name = query.value(1).toString();
        product.price = query.value(2).toDouble();
        product.quantity = query.value(3).toInt();
        return product;
    }
    return std::nullopt;
}

std::vector<Product> ProductRepository::getAllProducts() {
    std::vector<Product> products;
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.exec("SELECT product_id, name, price, quantity FROM products ORDER BY name");
    
    while (query.next()) {
        Product product;
        product.id = query.value(0).toInt();
        product.name = query.value(1).toString();
        product.price = query.value(2).toDouble();
        product.quantity = query.value(3).toInt();
        products.push_back(product);
    }
    return products;
}
```

### 3. Database Schema (add to DatabaseManager::initSchema())

```cpp
// In database/databasemanager.cpp, add to initSchema():

bool isSqlite = (m_db.driverName() == "QSQLITE");
QString autoInc = isSqlite ? "INTEGER PRIMARY KEY AUTOINCREMENT" : "INT AUTO_INCREMENT PRIMARY KEY";

query.exec(QString("CREATE TABLE IF NOT EXISTS products ("
           "product_id %1, "
           "name VARCHAR(200) NOT NULL, "
           "price DECIMAL(10,2) DEFAULT 0.0, "
           "quantity INT DEFAULT 0)").arg(autoInc));
```

### 4. UI Widget

```cpp
// ui/product/productsystem.h
#ifndef PRODUCTSYSTEM_H
#define PRODUCTSYSTEM_H

#include <QWidget>
#include <QTableView>
#include <QStandardItemModel>
#include <QPushButton>
#include "../../modules/product/productrepository.h"

class ProductSystem : public QWidget {
    Q_OBJECT
public:
    explicit ProductSystem(QWidget *parent = nullptr);

private slots:
    void loadProducts();
    void addProduct();
    void editProduct();
    void deleteProduct();

private:
    void setupUi();
    int getSelectedId();
    
    ProductRepository m_repository;
    QTableView *m_table;
    QStandardItemModel *m_model;
    QPushButton *m_btnAdd;
    QPushButton *m_btnEdit;
    QPushButton *m_btnDelete;
};

#endif // PRODUCTSYSTEM_H
```

```cpp
// ui/product/productsystem.cpp
#include "productsystem.h"
#include "modules/product/product.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QInputDialog>
#include <QDoubleValidator>

ProductSystem::ProductSystem(QWidget *parent) : QWidget(parent) {
    setupUi();
    loadProducts();
}

void ProductSystem::setupUi() {
    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(40, 30, 40, 30);
    layout->setSpacing(20);
    
    // Button Bar
    auto btnLayout = new QHBoxLayout();
    m_btnAdd = new QPushButton("Add Product");
    m_btnAdd->setProperty("type", "primary");
    m_btnEdit = new QPushButton("Edit");
    m_btnDelete = new QPushButton("Delete");
    m_btnDelete->setProperty("type", "danger");
    
    btnLayout->addWidget(m_btnAdd);
    btnLayout->addWidget(m_btnEdit);
    btnLayout->addWidget(m_btnDelete);
    btnLayout->addStretch();
    
    connect(m_btnAdd, &QPushButton::clicked, this, &ProductSystem::addProduct);
    connect(m_btnEdit, &QPushButton::clicked, this, &ProductSystem::editProduct);
    connect(m_btnDelete, &QPushButton::clicked, this, &ProductSystem::deleteProduct);
    
    // Table
    m_model = new QStandardItemModel(0, 4, this);
    m_model->setHorizontalHeaderLabels({"ID", "Name", "Price", "Quantity"});
    
    m_table = new QTableView(this);
    m_table->setModel(m_model);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->verticalHeader()->hide();
    m_table->setAlternatingRowColors(true);
    
    layout->addLayout(btnLayout);
    layout->addWidget(m_table);
}

void ProductSystem::loadProducts() {
    m_model->removeRows(0, m_model->rowCount());
    auto products = m_repository.getAllProducts();
    
    for (const auto& product : products) {
        int row = m_model->rowCount();
        m_model->insertRow(row);
        m_model->setData(m_model->index(row, 0), product.id);
        m_model->setData(m_model->index(row, 1), product.name);
        m_model->setData(m_model->index(row, 2), QString::number(product.price, 'f', 2));
        m_model->setData(m_model->index(row, 3), product.quantity);
    }
}

int ProductSystem::getSelectedId() {
    auto selection = m_table->selectionModel()->selectedRows();
    if (selection.isEmpty()) return -1;
    return m_model->data(selection.first().siblingAtColumn(0)).toInt();
}

void ProductSystem::addProduct() {
    bool ok;
    QString name = QInputDialog::getText(this, "Add Product", "Name:", QLineEdit::Normal, "", &ok);
    if (!ok || name.isEmpty()) return;
    
    double price = QInputDialog::getDouble(this, "Add Product", "Price:", 0, 0, 999999, 2, &ok);
    if (!ok) return;
    
    int quantity = QInputDialog::getInt(this, "Add Product", "Quantity:", 0, 0, 999999, 1, &ok);
    if (!ok) return;
    
    Product product;
    product.name = name;
    product.price = price;
    product.quantity = quantity;
    
    if (m_repository.addProduct(product)) {
        loadProducts();
        QMessageBox::information(this, "Success", "Product added successfully.");
    } else {
        QMessageBox::warning(this, "Error", "Failed to add product.");
    }
}

void ProductSystem::editProduct() {
    int id = getSelectedId();
    if (id < 0) {
        QMessageBox::warning(this, "No Selection", "Please select a product to edit.");
        return;
    }
    
    auto product = m_repository.getProductById(id);
    if (!product.has_value()) {
        QMessageBox::warning(this, "Error", "Product not found.");
        return;
    }
    
    bool ok;
    QString name = QInputDialog::getText(this, "Edit Product", "Name:", QLineEdit::Normal, product->name, &ok);
    if (!ok || name.isEmpty()) return;
    
    double price = QInputDialog::getDouble(this, "Edit Product", "Price:", product->price, 0, 999999, 2, &ok);
    if (!ok) return;
    
    int quantity = QInputDialog::getInt(this, "Edit Product", "Quantity:", product->quantity, 0, 999999, 1, &ok);
    if (!ok) return;
    
    product->name = name;
    product->price = price;
    product->quantity = quantity;
    
    if (m_repository.updateProduct(*product)) {
        loadProducts();
        QMessageBox::information(this, "Success", "Product updated successfully.");
    } else {
        QMessageBox::warning(this, "Error", "Failed to update product.");
    }
}

void ProductSystem::deleteProduct() {
    int id = getSelectedId();
    if (id < 0) {
        QMessageBox::warning(this, "No Selection", "Please select a product to delete.");
        return;
    }
    
    if (QMessageBox::question(this, "Confirm Delete", 
                               "Are you sure you want to delete this product?",
                               QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        if (m_repository.deleteProduct(id)) {
            loadProducts();
            QMessageBox::information(this, "Success", "Product deleted successfully.");
        } else {
            QMessageBox::warning(this, "Error", "Failed to delete product.");
        }
    }
}
```

### 5. Register in MainWindow

```cpp
// In mainwindow.h, add include:
#include "ui/product/productsystem.h"

// In mainwindow.cpp, in setupUi() method:
// Add menu item:
new QListWidgetItem("Products", m_sidebar);

// Add to content stack (use the helper lambda if you have it):
auto productSystem = new ProductSystem(this);
addModule(productSystem, "Products");
```

### 6. Update CMakeLists.txt

```cmake
set(PROJECT_SOURCES
    # ... existing files ...
    
    # Product Module
    modules/product/productrepository.h
    modules/product/productrepository.cpp
    ui/product/productsystem.h
    ui/product/productsystem.cpp
)
```

---

## Next Steps

1. **Replace "Product" with your entity name** (e.g., Task, Order, Customer)
2. **Update fields** in the struct and database schema
3. **Customize UI** - add more columns, filters, or advanced dialogs
4. **Add validation** in repository methods
5. **Implement search/filter** functionality

---

## Creating a Custom Dialog (Optional)

For better UX than `QInputDialog`, create a custom dialog:

```cpp
// ui/product/productdialog.h
#ifndef PRODUCTDIALOG_H
#define PRODUCTDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QPushButton>
#include "../../modules/product/product.h"

class ProductDialog : public QDialog {
    Q_OBJECT
public:
    explicit ProductDialog(QWidget *parent = nullptr);
    void setProduct(const Product& product);
    Product getProduct() const;

private:
    QLineEdit *m_nameEdit;
    QDoubleSpinBox *m_priceSpin;
    QSpinBox *m_quantitySpin;
};

#endif // PRODUCTDIALOG_H
```

```cpp
// ui/product/productdialog.cpp
#include "productdialog.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>

ProductDialog::ProductDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Product");
    setModal(true);
    resize(400, 200);
    
    auto layout = new QVBoxLayout(this);
    auto formLayout = new QFormLayout();
    
    m_nameEdit = new QLineEdit(this);
    m_priceSpin = new QDoubleSpinBox(this);
    m_priceSpin->setRange(0, 999999);
    m_priceSpin->setDecimals(2);
    m_quantitySpin = new QSpinBox(this);
    m_quantitySpin->setRange(0, 999999);
    
    formLayout->addRow("Name:", m_nameEdit);
    formLayout->addRow("Price:", m_priceSpin);
    formLayout->addRow("Quantity:", m_quantitySpin);
    
    auto btnBox = new QHBoxLayout();
    auto btnOk = new QPushButton("OK", this);
    btnOk->setProperty("type", "primary");
    auto btnCancel = new QPushButton("Cancel", this);
    
    connect(btnOk, &QPushButton::clicked, this, &QDialog::accept);
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
    
    btnBox->addStretch();
    btnBox->addWidget(btnOk);
    btnBox->addWidget(btnCancel);
    
    layout->addLayout(formLayout);
    layout->addLayout(btnBox);
}

void ProductDialog::setProduct(const Product& product) {
    m_nameEdit->setText(product.name);
    m_priceSpin->setValue(product.price);
    m_quantitySpin->setValue(product.quantity);
}

Product ProductDialog::getProduct() const {
    Product product;
    product.name = m_nameEdit->text();
    product.price = m_priceSpin->value();
    product.quantity = m_quantitySpin->value();
    return product;
}
```

Then use it in `ProductSystem`:

```cpp
#include "productdialog.h"

void ProductSystem::addProduct() {
    ProductDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        Product product = dialog.getProduct();
        if (m_repository.addProduct(product)) {
            loadProducts();
        }
    }
}

void ProductSystem::editProduct() {
    int id = getSelectedId();
    if (id < 0) return;
    
    auto product = m_repository.getProductById(id);
    if (!product.has_value()) return;
    
    ProductDialog dialog(this);
    dialog.setProduct(*product);
    if (dialog.exec() == QDialog::Accepted) {
        Product updated = dialog.getProduct();
        updated.id = id;
        if (m_repository.updateProduct(updated)) {
            loadProducts();
        }
    }
}
```

---

That's it! You now have a complete, working module. 🎉

