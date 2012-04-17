
// Qt includes
#include <QApplication>

// QtTesting includes
#include "pqTreeViewEventTranslator.h"
#include "pqEventTranslator.h"
#include "pqTestUtility.h"

#include "ctkTest.h"

// ----------------------------------------------------------------------------
class pqEventTranslatorTester : public QObject
{
  Q_OBJECT

private Q_SLOTS:

  void testDefaults();
  void testAddWidgetEventTranslator();
  void testGetRemoveWidgetEventTranslator();
  void testAddDefaultWidgetEventTranslators();
  void testAddDefaultWidgetEventTranslators_data();
};

// ----------------------------------------------------------------------------
void pqEventTranslatorTester::testDefaults()
{
  pqEventTranslator eventTranslator;

  QCOMPARE(eventTranslator.translators().count(), 0);
}

// ----------------------------------------------------------------------------
void pqEventTranslatorTester::testAddWidgetEventTranslator()
{
  pqEventTranslator eventTranslator;
  pqTreeViewEventTranslator* treeView = new pqTreeViewEventTranslator();
  eventTranslator.addWidgetEventTranslator(treeView);

  QCOMPARE(eventTranslator.translators().count(), 1);

  delete treeView;
}

// ----------------------------------------------------------------------------
void pqEventTranslatorTester::testGetRemoveWidgetEventTranslator()
{
  pqEventTranslator eventTranslator;
  pqTreeViewEventTranslator* treeView = new pqTreeViewEventTranslator();
  eventTranslator.addWidgetEventTranslator(treeView);

  QCOMPARE(eventTranslator.translators().count(), 1);
  QCOMPARE(eventTranslator.getWidgetEventTranslator(QString("pqTreeViewEventTranslator")),
           treeView);
  QCOMPARE(eventTranslator.removeWidgetEventTranslator(QString("pqTreeViewEventTranslator")),
           true);
  QCOMPARE(eventTranslator.translators().count(), 0);

  delete treeView;
}

// ----------------------------------------------------------------------------
void pqEventTranslatorTester::testAddDefaultWidgetEventTranslators()
{
  pqEventTranslator eventTranslator;
  pqTestUtility testUtility;
  eventTranslator.addDefaultWidgetEventTranslators(&testUtility);
  QList<pqWidgetEventTranslator*> translators = eventTranslator.translators();

  QFETCH(int, index);
  QFETCH(QString, widgetEventTranslatorName);

  QCOMPARE(QString(translators.at(index)->metaObject()->className()),
           widgetEventTranslatorName);
}

// ----------------------------------------------------------------------------
void pqEventTranslatorTester::testAddDefaultWidgetEventTranslators_data()
{
  QTest::addColumn<int>("index");
  QTest::addColumn<QString>("widgetEventTranslatorName");

  QTest::newRow("0") << 12 << "pqBasicWidgetEventTranslator";
  QTest::newRow("1") << 11 << "pqAbstractButtonEventTranslator";
  QTest::newRow("2") << 10 << "pqAbstractItemViewEventTranslator";
  QTest::newRow("3") << 9 << "pqAbstractSliderEventTranslator";
  QTest::newRow("4") << 8 << "pqComboBoxEventTranslator";
  QTest::newRow("5") << 7 << "pqDoubleSpinBoxEventTranslator";
  QTest::newRow("6") << 6 << "pqLineEditEventTranslator";
  QTest::newRow("7") << 5 << "pqMenuEventTranslator";
  QTest::newRow("8") << 4 << "pqSpinBoxEventTranslator";
  QTest::newRow("9") << 3 << "pqTabBarEventTranslator";
  QTest::newRow("10") << 2 << "pqTreeViewEventTranslator";
  QTest::newRow("11") << 1 << "pq3DViewEventTranslator";
  QTest::newRow("12") << 0 << "pqNativeFileDialogEventTranslator";
}

// ----------------------------------------------------------------------------
CTK_TEST_MAIN(pqEventTranslatorTest)
#include "moc_pqEventTranslatorTest.cpp"
