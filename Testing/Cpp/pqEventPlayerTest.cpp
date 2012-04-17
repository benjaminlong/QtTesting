
// Qt includes
#include <QApplication>

// QtTesting includes
#include "pqCommentEventPlayer.h"
#include "pqEventPlayer.h"
#include "pqTestUtility.h"

#include "ctkTest.h"

// ----------------------------------------------------------------------------
class pqEventPlayerTester: public QObject
{
  Q_OBJECT

private Q_SLOTS:

  void testDefaults();
  void testAddWidgetEventPlayer();
  void testGetRemoveWidgetEventPlayer();
  void testAddDefaultWidgetEventPlayers();
  void testAddDefaultWidgetEventPlayers_data();
};

// ----------------------------------------------------------------------------
void pqEventPlayerTester::testDefaults()
{
  pqEventPlayer eventPlayer;

  QCOMPARE(eventPlayer.players().count(), 0);
}

// ----------------------------------------------------------------------------
void pqEventPlayerTester::testAddWidgetEventPlayer()
{
  pqEventPlayer eventPlayer;

  pqCommentEventPlayer* commentPlayer = new pqCommentEventPlayer();
  eventPlayer.addWidgetEventPlayer(commentPlayer);

  QCOMPARE(eventPlayer.players().count(), 1);

  delete commentPlayer;

}

// ----------------------------------------------------------------------------
void pqEventPlayerTester::testGetRemoveWidgetEventPlayer()
{
  pqEventPlayer eventPlayer;

  pqCommentEventPlayer* commentPlayer = new pqCommentEventPlayer();
  eventPlayer.addWidgetEventPlayer(commentPlayer);

  QCOMPARE(eventPlayer.players().count(), 1);
  QCOMPARE(eventPlayer.getWidgetEventPlayer(QString("pqCommentEventPlayer")),
           commentPlayer);
  QCOMPARE(eventPlayer.removeWidgetEventPlayer(QString("pqCommentEventPlayer")),
           true);
  QCOMPARE(eventPlayer.players().count(), 0);

  delete commentPlayer;
}

// ----------------------------------------------------------------------------
void pqEventPlayerTester::testAddDefaultWidgetEventPlayers()
{
  pqEventPlayer eventPlayer;
  pqTestUtility testUtility;

  eventPlayer.addDefaultWidgetEventPlayers(&testUtility);
  QList<pqWidgetEventPlayer*> players = eventPlayer.players();

  QFETCH(QString, widgetEventPlayerName);
  QFETCH(int, index);

  QCOMPARE(QString(players.at(index)->metaObject()->className()),
           widgetEventPlayerName);
}

// ----------------------------------------------------------------------------
void pqEventPlayerTester::testAddDefaultWidgetEventPlayers_data()
{
  QTest::addColumn<int>("index");
  QTest::addColumn<QString>("widgetEventPlayerName");

  QTest::newRow("0") << 12 << "pqCommentEventPlayer";
  QTest::newRow("1") << 11 << "pqBasicWidgetEventPlayer";
  QTest::newRow("2") << 10 << "pqAbstractActivateEventPlayer";
  QTest::newRow("3") << 9 << "pqAbstractBooleanEventPlayer";
  QTest::newRow("4") << 8 << "pqAbstractDoubleEventPlayer";
  QTest::newRow("5") << 7 << "pqAbstractIntEventPlayer";
  QTest::newRow("6") << 6 << "pqAbstractItemViewEventPlayer";
  QTest::newRow("7") << 5 << "pqAbstractStringEventPlayer";
  QTest::newRow("8") << 4 << "pqTabBarEventPlayer";
  QTest::newRow("9") << 3 << "pqTreeViewEventPlayer";
  QTest::newRow("10") << 2 << "pqAbstractMiscellaneousEventPlayer";
  QTest::newRow("11") << 1 << "pq3DViewEventPlayer";
  QTest::newRow("12") << 0 << "pqNativeFileDialogEventPlayer";
}

// ----------------------------------------------------------------------------
CTK_TEST_MAIN(pqEventPlayerTest)
#include "moc_pqEventPlayerTest.cpp"

