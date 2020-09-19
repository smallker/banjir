const functions = require('firebase-functions')
const admin = require('firebase-admin').initializeApp()
const db = admin.database()
// // Create and Deploy Your First Cloud Functions
// // https://firebase.google.com/docs/functions/write-firebase-functions
//
// exports.helloWorld = functions.https.onRequest((request, response) => {
//   functions.logger.info("Hello logs!", {structuredData: true});
//   response.send("Hello from Firebase!");
// });

exports.fcm = functions.database.ref('/realtime/tembalang/status').onWrite((snapshot, context) => {
    var topic = 'status'
    var message = {
        notification: {
            title: "MONITORING BANJIR",
            body: snapshot.after
        },
        topic: topic
    }
    admin.messaging().send(message)
        .then((response) => {
            console.log('Successfully sent message:', response)
            return null
        })
        .catch((error) => {
            console.log('Error sending message:', error)
        })
    console.log(snapshot.after)
})