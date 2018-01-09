/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2018 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which (including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#include "customerGroup.h"

#include <QMessageBox>
#include <QSqlError>
#include <QVariant>

#include "crmacctcluster.h"
#include "customerGroup.h"
#include "errorReporter.h"
#include "guiErrorCheck.h"

customerGroup::customerGroup(QWidget* parent, const char* name, bool modal, Qt::WindowFlags fl)
    : XDialog(parent, name, modal, fl)
{
  setupUi(this);

  connect(_new, SIGNAL(clicked()), this, SLOT(sNew()));
  connect(_delete, SIGNAL(clicked()), this, SLOT(sDelete()));
  connect(_save, SIGNAL(clicked()), this, SLOT(sSaveClicked()));
  connect(_close, SIGNAL(clicked()), this, SLOT(sClose()));
  connect(_name, SIGNAL(editingFinished()), this, SLOT(sCheck()));

  _custgrpitem->addColumn(tr("Number"), _itemColumn, Qt::AlignLeft, true, "cust_number");
  _custgrpitem->addColumn(tr("Name"),   -1,          Qt::AlignLeft, true, "cust_name");
}

customerGroup::~customerGroup()
{
  // no need to delete child widgets, Qt does it all for us
}

void customerGroup::languageChange()
{
  retranslateUi(this);
}

enum SetResponse customerGroup::set(const ParameterList &pParams)
{
  XSqlQuery customeret;
  XDialog::set(pParams);
  QVariant param;
  bool     valid;

  param = pParams.value("groups_id", &valid);
  if (valid)
  {
    _custgrpid = param.toInt();
    populate();
  }

  param = pParams.value("mode", &valid);
  if (valid)
  {
    if (param.toString() == "new")
    {
      _mode = cNew;
      _saved = false;

      customeret.exec("SELECT NEXTVAL('groups_groups_id_seq') AS _groups_id;");
      if (customeret.first())
        _custgrpid = customeret.value("_groups_id").toInt();
      else if (ErrorReporter::error(QtCriticalMsg, this, tr("Error Retrieving Customer Group Information"),
                                    customeret, __FILE__, __LINE__))
      {
        return UndefinedError;
      }

      connect(_custgrpitem, SIGNAL(valid(bool)), _delete, SLOT(setEnabled(bool)));
    }
    else if (param.toString() == "edit")
    {
      _mode = cEdit;

      sFillList();

      connect(_custgrpitem, SIGNAL(valid(bool)), _delete, SLOT(setEnabled(bool)));
    }
    else if (param.toString() == "view")
    {
      _mode = cView;

      _name->setEnabled(false);
      _descrip->setEnabled(false);
      _new->setEnabled(false);
      _close->setText(tr("&Close"));
      _save->hide();

      sFillList();
    }
  }

  return NoError;
}

void customerGroup::sCheck()
{
  XSqlQuery customerCheck;
  _name->setText(_name->text().trimmed());
  if ((_mode == cNew) && (_name->text().length()))
  {
    customerCheck.prepare( "SELECT groups_id "
               "FROM custgrp "
               "WHERE (UPPER(groups_name)=UPPER(:groups_name));" );
    customerCheck.bindValue(":groups_name", _name->text());
    customerCheck.exec();
    if (customerCheck.first())
    {
      _custgrpid = customerCheck.value("groups_id").toInt();
      _mode = cEdit;
      populate();

      _name->setEnabled(false);
    }
    else if (ErrorReporter::error(QtCriticalMsg, this, tr("Error Retrieving Customer Group Information"),
                                  customerCheck, __FILE__, __LINE__))
    {
      return;
    }
  }
  _save->setEnabled(_name->text().length());
  _new->setEnabled(_name->text().length());
}

void customerGroup::sClose()
{
  XSqlQuery customerClose;
  if (_mode == cNew)
  {
    customerClose.prepare( "DELETE FROM custgrpitem "
               "WHERE (groupsitem_groups_id=:groups_id);"
               "DELETE FROM custgrp "
               "WHERE (groups_id=:groups_id);" );
    customerClose.bindValue(":groups_id", _custgrpid);
    customerClose.exec();
    if (customerClose.lastError().type() != QSqlError::NoError)
    {
      ErrorReporter::error(QtCriticalMsg, this, tr("Error Rejecting Customer Group"),
                           customerClose, __FILE__, __LINE__);
    }
  }

  reject();
}

void customerGroup::sSaveClicked()
{
  sSave(false);
}

void customerGroup::sSave(bool pPartial)
{
  XSqlQuery customerSave;

  QList<GuiErrorCheck>errors;
  errors<<GuiErrorCheck(_name->text().trimmed().isEmpty(), _name,
                        tr("You cannot have an empty name."));

  if(GuiErrorCheck::reportErrors(this,tr("Cannot Save Customer Group"),errors))
      return;

  customerSave.prepare("SELECT groups_id"
            "  FROM custgrp"
            " WHERE((groups_name=:groups_name)"
            "   AND (groups_id != :groups_id))");
  customerSave.bindValue(":groups_id", _custgrpid);
  customerSave.bindValue(":groups_name", _name->text());
  customerSave.exec();
  if(customerSave.first())
  {
    QMessageBox::warning(this, tr("Cannot Save Customer Group"),
      tr("You cannot have a duplicate name."));
    _name->setFocus();
    return;
  }

  if (_mode == cNew && !_saved)
    customerSave.prepare( "INSERT INTO custgrp "
               "(groups_id, groups_name, groups_descrip) "
               "VALUES "
               "(:groups_id, :groups_name, :groups_descrip);" );
  else if (_mode == cEdit)
    customerSave.prepare( "UPDATE custgrp "
               "SET groups_name=:groups_name, groups_descrip=:groups_descrip "
               "WHERE (groups_id=:groups_id);" );

  customerSave.bindValue(":groups_id", _custgrpid);
  customerSave.bindValue(":groups_name", _name->text());
  customerSave.bindValue(":groups_descrip", _descrip->text().trimmed());
  customerSave.exec();
  if (ErrorReporter::error(QtCriticalMsg, this, tr("Error Saving Customer Group"),
                                customerSave, __FILE__, __LINE__))
  {
    return;
  }

  _saved = true;

  if (!pPartial)
    done(_custgrpid);
}

void customerGroup::sDelete()
{
  XSqlQuery customerDelete;
  customerDelete.prepare( "DELETE FROM custgrpitem "
             "WHERE (groupsitem_id=:custgrpitem_id);" );
  customerDelete.bindValue(":custgrpitem_id", _custgrpitem->id());
  customerDelete.exec();
  if (ErrorReporter::error(QtCriticalMsg, this, tr("Error Deleting Customer From Group"),
                                customerDelete, __FILE__, __LINE__))
  {
    return;
  }

  sFillList();
}

void customerGroup::sNew()
{
  XSqlQuery customerNew;
  ParameterList params;

  if (!_saved)
    sSave(true);

  CRMAcctSearch *newdlg = new CRMAcctSearch(this);
  newdlg->setSubtype(CRMAcctLineEdit::Cust);

  int custid;
  if ((custid = newdlg->exec()) != XDialog::Rejected)
  {
    if (custid == -1)
      return;

    customerNew.prepare( "SELECT groupsitem_id "
               "FROM custgrpitem "
               "WHERE ( (groupsitem_groups_id=:groupsitem_groups_id)"
               " AND (groupsitem_reference_id=:groupsitem_reference_id) );" );
    customerNew.bindValue(":groupsitem_groups_id", _custgrpid);
    customerNew.bindValue(":groupsitem_reference_id", custid);
    customerNew.exec();
    if (customerNew.first())
      return;
    else if (ErrorReporter::error(QtCriticalMsg, this, tr("Error Adding Customer To Group"),
                                  customerNew, __FILE__, __LINE__))
    {
      return;
    }

    customerNew.prepare( "INSERT INTO custgrpitem "
               "(groupsitem_groups_id, groupsitem_reference_id) "
               "VALUES "
               "(:groupsitem_groups_id, :groupsitem_reference_id);" );
    customerNew.bindValue(":groupsitem_groups_id", _custgrpid);
    customerNew.bindValue(":groupsitem_reference_id", custid);
    customerNew.exec();
    if (ErrorReporter::error(QtCriticalMsg, this, tr("Error Adding Customer To Group"),
                                  customerNew, __FILE__, __LINE__))
    {
      return;
    }

    sFillList();
  }
}

void customerGroup::sFillList()
{
  XSqlQuery customerFillList;
  customerFillList.prepare( "SELECT groupsitem_id, cust_number, cust_name "
             "FROM custgrpitem, custinfo "
             "WHERE ( (groupsitem_reference_id=cust_id) "
             " AND (groupsitem_groups_id=:groups_id) ) "
             "ORDER BY cust_number;" );
  customerFillList.bindValue(":groups_id", _custgrpid);
  customerFillList.exec();
  _custgrpitem->populate(customerFillList);
  if (ErrorReporter::error(QtCriticalMsg, this, tr("Error Retrieving Customer Group Information"),
                                customerFillList, __FILE__, __LINE__))
  {
    return;
  }
}

void customerGroup::populate()
{
  XSqlQuery customerpopulate;
  customerpopulate.prepare( "SELECT groups_name, groups_descrip "
             "FROM custgrp "
             "WHERE (groups_id=:groups_id);" );
  customerpopulate.bindValue(":groups_id", _custgrpid);
  customerpopulate.exec();
  if (customerpopulate.first())
  {
    _name->setText(customerpopulate.value("groups_name").toString());
    _descrip->setText(customerpopulate.value("groups_descrip").toString());
    _save->setEnabled(true);
    _new->setEnabled(true);
  }
  else if (ErrorReporter::error(QtCriticalMsg, this, tr("Error Retrieving Customer Group Information"),
                                customerpopulate, __FILE__, __LINE__))
  {
    return;
  }
}
