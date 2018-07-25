/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2018 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which (including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#include "scriptapi_internal.h"
#include "qwebenginesettingsproto.h"

QScriptValue FontFamilyToScriptValue(QScriptEngine *engine, const QWebEngineSettings::FontFamily &item)
{
  return engine->newVariant(item);
}
void FontFamilyFromScriptValue(const QScriptValue &obj, QWebEngineSettings::FontFamily &item)
{
  item = (QWebEngineSettings::FontFamily)obj.toInt32();
}

QScriptValue FontSizeToScriptValue(QScriptEngine *engine, const QWebEngineSettings::FontSize &item)
{
  return engine->newVariant(item);
}
void FontSizeFromScriptValue(const QScriptValue &obj, QWebEngineSettings::FontSize &item)
{
  item = (QWebEngineSettings::FontSize)obj.toInt32();
}

QScriptValue WebAttributeToScriptValue(QScriptEngine *engine, const QWebEngineSettings::WebAttribute &item)
{
  return engine->newVariant(item);
}
void WebAttributeFromScriptValue(const QScriptValue &obj, QWebEngineSettings::WebAttribute &item)
{
  item = (QWebEngineSettings::WebAttribute)obj.toInt32();
}

QScriptValue globalSettingsForJS(QScriptContext* context, QScriptEngine* engine)
{
  Q_UNUSED(context);
  return engine->toScriptValue(QWebEngineSettings::globalSettings());
}

void setupQWebEngineSettingsProto(QScriptEngine *engine)
{
  QScriptValue::PropertyFlags permanent = QScriptValue::ReadOnly | QScriptValue::Undeletable;

  QScriptValue proto = engine->newQObject(new QWebEngineSettingsProto(engine));
  engine->setDefaultPrototype(qMetaTypeId<QWebEngineSettings*>(), proto);

  QScriptValue constructor = engine->newFunction(constructQWebEngineSettings, proto);
  engine->globalObject().setProperty("QWebEngineSettings", constructor);

  qScriptRegisterMetaType(engine, FontFamilyToScriptValue, FontFamilyFromScriptValue);
  constructor.setProperty("StandardFont", QScriptValue(engine, QWebEngineSettings::StandardFont), permanent);
  constructor.setProperty("FixedFont", QScriptValue(engine, QWebEngineSettings::FixedFont), permanent);
  constructor.setProperty("SerifFont", QScriptValue(engine, QWebEngineSettings::SerifFont), permanent);
  constructor.setProperty("SansSerifFont", QScriptValue(engine, QWebEngineSettings::SansSerifFont), permanent);
  constructor.setProperty("CursiveFont", QScriptValue(engine, QWebEngineSettings::CursiveFont), permanent);
  constructor.setProperty("FantasyFont", QScriptValue(engine, QWebEngineSettings::FantasyFont), permanent);

  qScriptRegisterMetaType(engine, FontSizeToScriptValue, FontSizeFromScriptValue);
  constructor.setProperty("MinimumFontSize", QScriptValue(engine, QWebEngineSettings::MinimumFontSize), permanent);
  constructor.setProperty("MinimumLogicalFontSize", QScriptValue(engine, QWebEngineSettings::MinimumLogicalFontSize), permanent);
  constructor.setProperty("DefaultFontSize", QScriptValue(engine, QWebEngineSettings::DefaultFontSize), permanent);
  constructor.setProperty("DefaultFixedFontSize", QScriptValue(engine, QWebEngineSettings::DefaultFixedFontSize), permanent);

  qScriptRegisterMetaType(engine, WebAttributeToScriptValue, WebAttributeFromScriptValue);
  constructor.setProperty("AutoLoadImages", QScriptValue(engine, QWebEngineSettings::AutoLoadImages), permanent);
  constructor.setProperty("JavascriptEnabled", QScriptValue(engine, QWebEngineSettings::JavascriptEnabled), permanent);
  constructor.setProperty("PluginsEnabled", QScriptValue(engine, QWebEngineSettings::PluginsEnabled), permanent);
  constructor.setProperty("JavascriptCanOpenWindows", QScriptValue(engine, QWebEngineSettings::JavascriptCanOpenWindows), permanent);
  constructor.setProperty("JavascriptCanAccessClipboard", QScriptValue(engine, QWebEngineSettings::JavascriptCanAccessClipboard), permanent);
  constructor.setProperty("SpatialNavigationEnabled", QScriptValue(engine, QWebEngineSettings::SpatialNavigationEnabled), permanent);
  constructor.setProperty("LinksIncludedInFocusChain", QScriptValue(engine, QWebEngineSettings::LinksIncludedInFocusChain), permanent);
  constructor.setProperty("PrintElementBackgrounds", QScriptValue(engine, QWebEngineSettings::PrintElementBackgrounds), permanent);
  constructor.setProperty("LocalStorageEnabled", QScriptValue(engine, QWebEngineSettings::LocalStorageEnabled), permanent);
  constructor.setProperty("LocalContentCanAccessRemoteUrls", QScriptValue(engine, QWebEngineSettings::LocalContentCanAccessRemoteUrls), permanent);
  constructor.setProperty("LocalContentCanAccessFileUrls", QScriptValue(engine, QWebEngineSettings::LocalContentCanAccessFileUrls), permanent);
  constructor.setProperty("XSSAuditingEnabled", QScriptValue(engine, QWebEngineSettings::XSSAuditingEnabled), permanent);
  constructor.setProperty("ScrollAnimatorEnabled", QScriptValue(engine, QWebEngineSettings::ScrollAnimatorEnabled), permanent);
  constructor.setProperty("Accelerated2dCanvasEnabled", QScriptValue(engine, QWebEngineSettings::Accelerated2dCanvasEnabled), permanent);
  constructor.setProperty("WebGLEnabled", QScriptValue(engine, QWebEngineSettings::WebGLEnabled), permanent);
  constructor.setProperty("HyperlinkAuditingEnabled", QScriptValue(engine, QWebEngineSettings::HyperlinkAuditingEnabled), permanent);

  QScriptValue globalSettings = engine->newFunction(globalSettingsForJS);
  constructor.setProperty("globalSettings", globalSettings);

  // TODO: Can't seem to get this working. Something is wrong with how we expose QUrl.
  /*
  QScriptValue iconForUrl = engine->newFunction(iconForUrlForJS);
  constructor.setProperty("iconForUrl", iconForUrl);
  */

  // TODO: Expose QPixmap for this to work.
  /*
  QScriptValue webGraphic = engine->newFunction(webGraphicForJS);
  constructor.setProperty("webGraphic", webGraphic);
  */
}

QScriptValue constructQWebEngineSettings(QScriptContext * /*context*/, QScriptEngine  *engine)
{
  QWebEngineSettings *obj = 0;
  // TODO: QWebEngineSettings does not have a constructor.
  //obj = new QWebEngineSettings();
  return engine->toScriptValue(obj);
}

QWebEngineSettingsProto::QWebEngineSettingsProto(QObject *parent)
    : QObject(parent)
{
}

QString QWebEngineSettingsProto::defaultTextEncoding() const
{
  QWebEngineSettings *item = qscriptvalue_cast<QWebEngineSettings*>(thisObject());
  if (item)
    return item->defaultTextEncoding();
  return QString();
}

QString QWebEngineSettingsProto::fontFamily(QWebEngineSettings::FontFamily which) const
{
  QWebEngineSettings *item = qscriptvalue_cast<QWebEngineSettings*>(thisObject());
  if (item)
    return item->fontFamily(which);
  return QString();
}

int QWebEngineSettingsProto::fontSize(QWebEngineSettings::FontSize type) const
{
  QWebEngineSettings *item = qscriptvalue_cast<QWebEngineSettings*>(thisObject());
  if (item)
    return item->fontSize(type);
  return 0;
}

void QWebEngineSettingsProto::resetAttribute(QWebEngineSettings::WebAttribute attribute)
{
  QWebEngineSettings *item = qscriptvalue_cast<QWebEngineSettings*>(thisObject());
  if (item)
    item->resetAttribute(attribute);
}

void QWebEngineSettingsProto::resetFontFamily(QWebEngineSettings::FontFamily which)
{
  QWebEngineSettings *item = qscriptvalue_cast<QWebEngineSettings*>(thisObject());
  if (item)
    item->resetFontFamily(which);
}

void QWebEngineSettingsProto::resetFontSize(QWebEngineSettings::FontSize type)
{
  QWebEngineSettings *item = qscriptvalue_cast<QWebEngineSettings*>(thisObject());
  if (item)
    item->resetFontSize(type);
}

void QWebEngineSettingsProto::setAttribute(QWebEngineSettings::WebAttribute attribute, bool on)
{
  QWebEngineSettings *item = qscriptvalue_cast<QWebEngineSettings*>(thisObject());
  if (item)
    item->setAttribute(attribute, on);
}

void QWebEngineSettingsProto::setDefaultTextEncoding(const QString & encoding)
{
  QWebEngineSettings *item = qscriptvalue_cast<QWebEngineSettings*>(thisObject());
  if (item)
    item->setDefaultTextEncoding(encoding);
}

void QWebEngineSettingsProto::setFontFamily(QWebEngineSettings::FontFamily which, const QString & family)
{
  QWebEngineSettings *item = qscriptvalue_cast<QWebEngineSettings*>(thisObject());
  if (item)
    item->setFontFamily(which, family);
}

void QWebEngineSettingsProto::setFontSize(QWebEngineSettings::FontSize type, int size)
{
  QWebEngineSettings *item = qscriptvalue_cast<QWebEngineSettings*>(thisObject());
  if (item)
    item->setFontSize(type, size);
}

bool QWebEngineSettingsProto::testAttribute(QWebEngineSettings::WebAttribute attribute) const
{
  QWebEngineSettings *item = qscriptvalue_cast<QWebEngineSettings*>(thisObject());
  if (item)
    return item->testAttribute(attribute);
  return false;
}
