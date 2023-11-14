//// MicrosoftOAuth2Test.cpp

////#include <QtTest>
//#include "MicrosoftOAuth2.h"

//class MicrosoftOAuth2Test : public QObject {
//    Q_OBJECT

//private slots:
//    void testFailedAuthentication_InvalidCredentials();

//private:
//    MicrosoftOAuth2 microsoftAuth;

//private slots:
//    void initTestCase() {
//        // Set up the OAuth2 configuration for testing
//        microsoftAuth.setClientId("test_client_id");
//        microsoftAuth.setClientSecret("test_client_secret");
//        microsoftAuth.setRedirectUri("http://localhost:3000/authdone");
//    }
//};

//void MicrosoftOAuth2Test::testFailedAuthentication_InvalidCredentials() {
//    // Connect to the authenticationError signal
//    QSignalSpy errorSpy(&microsoftAuth, &MicrosoftOAuth2::authenticationError);

//    // Simulate a failed authentication with invalid client credentials
//    // This can be done by temporarily modifying the client credentials in the initTestCase.

//    // Start the authentication process
//    microsoftAuth.authenticate();

//    // Ensure that the authenticationError signal is emitted
//    QCOMPARE(errorSpy.count(), 1);

//    // Verify the error message
//    const QList<QVariant> arguments = errorSpy.at(0);
//    QVERIFY(!arguments.isEmpty());
//    const QString errorMessage = arguments.at(0).toString();
//    QVERIFY(!errorMessage.isEmpty());
//    QVERIFY(errorMessage.contains("invalid client credentials")); // Adjust this as needed
//}

////QTEST_APPLESS_MAIN(MicrosoftOAuth2Test)

//#include "MicrosoftOAuth2Test.moc"
