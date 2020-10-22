const functions = require('firebase-functions')
const admin = require('firebase-admin').initializeApp()
const db = admin.database()

/// Express JS middleware config
const express = require('express')
const app = express()
const main = express()
const cors = require('cors')
const bodyParser = require('body-parser')
main.use('/', app)
main.use(cors())
main.use(bodyParser.json())
main.use(express.urlencoded({ extended: true }))

process.env.TZ = 'Asia/Jakarta'
exports.fcm = functions.database.ref('/realtime/tembalang/status').onWrite((snapshot, context) => {
    db.ref('/realtime/tembalang/tinggi').once('value', (data) => {
        let topic = 'status'
        let status = snapshot.after.val()
        let tinggi = data.val()
        let message = {
            notification: {
                title: "MONITORING BANJIR",
                body: `Status : ${status}   Tinggi : ${tinggi} m`
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
    })
})

exports.api = functions.https.onRequest(main)
app.post('/sensor', (req, res) => {
    let debit = req.body.debit
    let hujan = req.body.hujan
    let tinggi = req.body.tinggi
    let intensitas = req.body.intensitas==null?0:req.body.intensitas
    let dateNow = new Date(Date.now())
    let year = dateNow.getFullYear()
    let month = dateNow.getMonth() < 10 ? "0" + dateNow.getMonth() : dateNow.getMonth()
    let date = dateNow.getDate() < 10 ? "0" + dateNow.getDate() : dateNow.getDate()
    let hour = dateNow.getHours() <10 ? "0" + dateNow.getHours() : dateNow.getHours()
    let minute = dateNow.getMinutes() < 10 ? "0" + dateNow.getMinutes() : dateNow.getMinutes()
    let second = dateNow.getSeconds()

    // formatted time
    let formattedTime = `${year}-${month}-${date} ${hour}:${minute}`
    let jam = `${hour}:${minute}:${second}`
    let tanggal = `${year}-${month}-${date}`

    let status = ""
    if (tinggi < 0.2) status = "Aman"
    if (tinggi > 0.4 && tinggi < 0.6) status = "Siaga"
    if (tinggi > 0.6) status = "Bahaya"
    db.ref('/realtime/tembalang/').update({
        hujan: hujan,
        jamtanggal: formattedTime,
        debit: debit,
        status: status,
        tinggi: tinggi,
        intensitas: intensitas
    })
    db.ref('/sensor/tembalang/menit/').push({
        hujan: hujan,
        jamtanggal: formattedTime,
        debit: debit,
        status: status,
        tinggi: tinggi,
        intensitas: intensitas
    })
    if (minute == 0) {
        db.ref('/sensor/tembalang/jam/').push({
            hujan: hujan,
            jamtanggal: formattedTime,
            debit: debit,
            status: status,
            tinggi: tinggi,
            intensitas: intensitas
        })
    }
    // if (hour == 0) {
    //     db.ref('/sensor/tembalang/hari/').push({
    //         hujan: hujan,
    //         jamtanggal: formattedTime,
    //         debit: debit,
    //         status: status,
    //         tinggi: tinggi,
    //         intensitas: intensitas
    //     })
    // }
    res.send("OK")
})