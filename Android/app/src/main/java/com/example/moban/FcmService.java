package com.example.moban;

import android.annotation.SuppressLint;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.os.Build;
import android.util.Log;
import androidx.core.app.NotificationCompat;
import androidx.core.app.NotificationManagerCompat;
import com.google.firebase.messaging.FirebaseMessagingService;
import com.google.firebase.messaging.RemoteMessage;

@SuppressLint("MissingFirebaseInstanceTokenRefresh")
public class FcmService extends FirebaseMessagingService {
    String CHANNEL_ID = "notif_app";
    int NOTIFICATION_ID = 999;
    String CHANNEL_NAME = "Tugas Akhir";
    String CHANNEL_DESC = "Notif";
    @Override
    public void onMessageReceived(RemoteMessage remoteMessage) {
        String TAG = "testing";
        Log.d(TAG, "From: " + remoteMessage.getFrom());
        if (remoteMessage.getData().size() > 0) {
            // handle fcm data
            Log.d(TAG, "Message data payload: " + remoteMessage.getData());
        }

        if (remoteMessage.getNotification() != null) {
            // handle notification
            String status = remoteMessage.getNotification().getBody();
            showNotification(status);
            Log.d(TAG, "Message Notification Body: " + remoteMessage.getNotification().getBody());
        }
    }
    private void showNotification(String status) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            NotificationChannel channel = new NotificationChannel(CHANNEL_ID, CHANNEL_NAME, NotificationManager.IMPORTANCE_DEFAULT);
            channel.setDescription(CHANNEL_DESC);
            NotificationManager manager = getSystemService(NotificationManager.class);
            manager.createNotificationChannel(channel);
        }
        NotificationCompat.Builder builder = new NotificationCompat.Builder(FcmService.this, CHANNEL_ID);
        builder.setSmallIcon(R.drawable.logomoban);
        builder.setContentTitle("NOTIFIKASI PERINGATAN!");
        builder.setContentText("Status " + status);
        builder.setPriority(NotificationCompat.PRIORITY_DEFAULT);
        NotificationManagerCompat notificationManagerCompat = NotificationManagerCompat.from(FcmService.this);
        notificationManagerCompat.notify(NOTIFICATION_ID, builder.build());
    }
}
