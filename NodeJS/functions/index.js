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

exports.fcm = functions.database.ref('/realtime/tembalang/status').onUpdate((snapshot, context) => {
    console.log(snapshot.after())
})