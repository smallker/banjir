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


exports.fcm = functions.database.ref('/realtime/tembalang/status').onWrite((snapshot, context) => {
    db.ref('/realtime/tembalang/tinggi').once('value', (data) => {
        let topic = 'status'
        let status = snapshot.after.val()
        let tinggi = data.val()
        let message = {
            notification: {
                title: "MONITORING BANJIR",
                body: `Status : ${status}   Tinggi : ${tinggi} cm`
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
    
    let query = req.query.type
    let debit = req.body.debit
    let hujan = req.body.hujan
    let tinggi = req.body.tinggi
    let dateNow = new Date(Date.now())
    let year = dateNow.getFullYear()
    let month = dateNow.getMonth()
    let date = dateNow.getDate()
    let hour = dateNow.getHours()
    let minute = dateNow.getMinutes()
    let second = dateNow.getSeconds()

    // formatted time
    let formattedTime = `${year}-${month}-${date} ${hour}:${minute}:${second}`
    let jam = `${hour}:${minute}:${second}`
    let tanggal = `${year}-${month}-${date}`

    switch (query) {
        case 'hujan':
            db.ref('/realtime/tembalang/').update({
                hujan: hujan,
                jamtanggal: formattedTime
            })
            db.ref('/sensor/tembalang/hujan').push({
                nilai: hujan,
                jam: jam,
                tanggal: tanggal
            })
            res.send('hujan')
            break
        default:
            // status
            let status = ''
            if(tinggi > 40) status = 'Bahaya'
            else if (tinggi > 20 && tinggi < 40) status = 'Siaga'
            else status = 'Aman'
            db.ref('/realtime/tembalang').update({
                jamtanggal: formattedTime,
                debit: debit,
                status: status,
                tinggi: tinggi
            })
            db.ref('/sensor/tembalang/debit').push({
                debit: debit,
                tinggi: tinggi,
                jam: jam,
                tanggal: tanggal
            })
            res.send('realtime')
            break
    }
})