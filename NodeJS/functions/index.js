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

exports.timestampNodeMcu = functions.database.ref('/sensor/tembalang/menit/{pushId}/timestamp')
    .onCreate((snapshot, context) => {
      return snapshot.ref.parent.child('timestamp').set(Date.now());
    });

exports.api = functions.https.onRequest(main)
app.post('/sensor', (req, res) => {
    let month_arr = ["01","02","03","04","05","06","07","08","09","10","11","12"]
    let debit = req.body.debit
    let hujan = req.body.hujan
    let tinggi = req.body.tinggi
    let intensitas = req.body.intensitas == null ? 0 : req.body.intensitas
    let dateNow = new Date(Date.now())
    let year = dateNow.getFullYear()
    // let month = dateNow.getMonth() < 10 ? "0" + dateNow.getMonth() : dateNow.getMonth()
    let month = month_arr[dateNow.getMonth()]
    let date = dateNow.getDate() < 10 ? "0" + dateNow.getDate() : dateNow.getDate()
    let hour = dateNow.getHours() < 10 ? "0" + dateNow.getHours() : dateNow.getHours()
    let minute = dateNow.getMinutes() < 10 ? "0" + dateNow.getMinutes() : dateNow.getMinutes()
    let second = dateNow.getSeconds()

    // intensitas
    // let lastHour = new Date(Date.now()- Date.)
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
        intensitas: intensitas,
        timestamp: Date.now()
    })
    db.ref('/sensor/tembalang/menit/').push({
        hujan: hujan,
        jam: jam,
        tanggal: tanggal,
        debit: debit,
        status: status,
        tinggi: tinggi,
        intensitas: intensitas,
        timestamp: Date.now()
    })
    res.send("OK")
})


app.get('/hourly', (req, res) => {
    let dateNow = new Date(Date.now())
    let year = dateNow.getFullYear()
    let month = dateNow.getMonth() < 10 ? "0" + dateNow.getMonth() : dateNow.getMonth()
    let date = dateNow.getDate() < 10 ? "0" + dateNow.getDate() : dateNow.getDate()
    let hour = dateNow.getHours() < 10 ? "0" + dateNow.getHours() : dateNow.getHours()
    let minute = dateNow.getMinutes() < 10 ? "0" + dateNow.getMinutes() : dateNow.getMinutes()
    let second = dateNow.getSeconds()

    // intensitas
    // let lastHour = new Date(Date.now()- Date.)
    // formatted time
    let formattedTime = `${year}-${month}-${date} ${hour}:${minute}`
    let jam = `${hour}:${minute}:${second}`
    let tanggal = `${year}-${month}-${date}`

    let lastHour = Date.now() - 3600000// jam sekarang - 1 jam kebelakang
    let intensitasJam = 0
    let hujan = 0
    let debit = []
    let tinggi = []
    db.ref('/sensor/tembalang/menit').orderByChild('timestamp').startAt(lastHour).endAt(Date.now())
        .once('value', (snapshot) => {
            let numchildren = snapshot.numChildren()
            let count = 0
            let future = new Promise((resolve, reject) => {
                snapshot.forEach((value) => {
                    console.log(snapshot.val().intensitas)
                    intensitasJam = intensitasJam + value.val().intensitas
                    hujan = hujan + value.val().hujan
                    debit.push(value.val().debit)
                    tinggi.push(value.val().tinggi)
                    count++
                })
                if (count == numchildren) resolve()
                if (count < 0) reject(Error)
            })
            future.then((_) => {
                console.log(intensitasJam);
                db.ref('/realtime/tembalang/').once('value', (snapshot) => {
                    db.ref('/sensor/tembalang/jam').push(snapshot.val())
                        .then((snap) => {
                            db.ref(`/sensor/tembalang/jam/${snap.key}/`).update(
                                {
                                    intensitas: intensitasJam,
                                    hujan: hujan,
                                    debit: Math.max.apply(null, debit),
                                    tinggi: Math.max.apply(null, tinggi),
                                    jam: jam,
                                    tanggal: tanggal,
                                    timestamp: Date.now()
                                })
                        }).catch((Error) => console.log(Error))
                })
            })
        })
    res.send('OK')
})

app.get('/daily', (req, res) => {
    let dateNow = new Date(Date.now())
    let year = dateNow.getFullYear()
    let month = dateNow.getMonth() < 10 ? "0" + dateNow.getMonth() : dateNow.getMonth()
    let date = dateNow.getDate() < 10 ? "0" + dateNow.getDate() : dateNow.getDate()
    let hour = dateNow.getHours() < 10 ? "0" + dateNow.getHours() : dateNow.getHours()
    let minute = dateNow.getMinutes() < 10 ? "0" + dateNow.getMinutes() : dateNow.getMinutes()
    let second = dateNow.getSeconds()

    // intensitas
    // let lastHour = new Date(Date.now()- Date.)
    // formatted time
    let formattedTime = `${year}-${month}-${date} ${hour}:${minute}`
    let jam = `${hour}:${minute}:${second}`
    let tanggal = `${year}-${month}-${date}`

    let lastHour = Date.now() - 86400000// jam sekarang - 1 jam kebelakang
    let intensitasJam = 0
    let hujan = 0
    let debit = []
    let tinggi = []
    db.ref('/sensor/tembalang/jam').orderByChild('timestamp').startAt(lastHour).endAt(Date.now())
        .once('value', (snapshot) => {
            let numchildren = snapshot.numChildren()
            let count = 0
            let future = new Promise((resolve, reject) => {
                snapshot.forEach((value) => {
                    console.log(snapshot.val().intensitas)
                    intensitasJam = intensitasJam + value.val().intensitas
                    hujan = hujan + value.val().hujan
                    debit.push(value.val().debit)
                    tinggi.push(value.val().tinggi)
                    count++
                })
                if (count == numchildren) resolve()
                if (count < 0) reject(Error)
            })
            future.then((_) => {
                console.log(`intensitasJam ${intensitasJam}`);
                db.ref('/realtime/tembalang/').once('value', (snapshot) => {
                    db.ref('/sensor/tembalang/hari').push(snapshot.val())
                        .then((snap) => {
                            db.ref(`/sensor/tembalang/hari/${snap.key}/`).update(
                                {
                                    intensitas: intensitasJam,
                                    hujan: hujan,
                                    debit: Math.max.apply(null, debit),
                                    tinggi: Math.max.apply(null, tinggi),
                                    jam: jam,
                                    tanggal: tanggal,
                                    timestamp: Date.now()
                                })
                        }).catch((Error) => console.log(Error))
                })
            })
        })
    res.send('OK')
})