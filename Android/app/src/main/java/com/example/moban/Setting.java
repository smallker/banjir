package com.example.moban;

import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.content.Context;
import android.content.SharedPreferences;
import android.os.Build;
import android.os.Bundle;
import android.widget.CompoundButton;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.SwitchCompat;
import androidx.core.app.NotificationCompat;
import androidx.core.app.NotificationManagerCompat;

import com.google.android.gms.tasks.OnSuccessListener;
import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseError;
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;
import com.google.firebase.database.ValueEventListener;
import com.google.firebase.messaging.FirebaseMessaging;


public class Setting extends AppCompatActivity {
    String CHANNEL_ID = "notif_app";
    int NOTIFICATION_ID = 999;
    String CHANNEL_NAME = "Tugas Akhir";
    String CHANNEL_DESC = "Notif";
    TextView statusnotif;
    DatabaseReference reff;

    private SharedPreferences notifSet;
    private static final String NOTIF = "Notif";

    private SwitchCompat tb_notif;

    private String statuss = "";


    private void showNotification(String status) {
        //Pengaturan notifikasi
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            NotificationChannel channel = new NotificationChannel(CHANNEL_ID, CHANNEL_NAME, NotificationManager.IMPORTANCE_DEFAULT);
            channel.setDescription(CHANNEL_DESC);
            NotificationManager manager = getSystemService(NotificationManager.class);
            manager.createNotificationChannel(channel);
        }
        NotificationCompat.Builder builder = new NotificationCompat.Builder(Setting.this, CHANNEL_ID);
        builder.setSmallIcon(R.drawable.logomoban);
        builder.setContentTitle("NOTIFIKASI PERINGATAN!");
        builder.setContentText("Status " + status);
        builder.setPriority(NotificationCompat.PRIORITY_DEFAULT);

        NotificationManagerCompat notificationManagerCompat = NotificationManagerCompat.from(Setting.this);
        notificationManagerCompat.notify(NOTIFICATION_ID, builder.build());

        Toast.makeText(getApplicationContext(), "NOTIFIKASI HIDUP", Toast.LENGTH_SHORT).show();
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_setting);
        tb_notif = findViewById(R.id.btn_notif);


        notifSet = getSharedPreferences(NOTIF, Context.MODE_PRIVATE);
        tb_notif.setChecked(notifSet.getBoolean(NOTIF, false));

//        reff = FirebaseDatabase.getInstance().getReference();
//        reff.child("realtime").child("tembalang").addValueEventListener(new ValueEventListener() {
//            @Override
//            public void onDataChange(@NonNull DataSnapshot dataSnapshot) {
//                statuss =dataSnapshot.child("status").getValue().toString();
//            }
//
//            @Override
//            public void onCancelled(@NonNull DatabaseError databaseError) {
//
//            }
//        });


        tb_notif.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if (isChecked) {
                    FirebaseMessaging.getInstance().subscribeToTopic("status").addOnSuccessListener(new OnSuccessListener<Void>() {
                        @Override
                        public void onSuccess(Void aVoid) {
//                            showNotif(statuss);
                            //Untuk mengaktifkan kondisi state ON
                            SharedPreferences.Editor editor = getSharedPreferences(NOTIF, MODE_PRIVATE).edit();
                            editor.putBoolean(NOTIF, true);
                            editor.apply();
                            Toast.makeText(getApplicationContext(), "NOTIFIKASI HIDUP", Toast.LENGTH_SHORT).show();
                        }
                    });
                } else {
                    FirebaseMessaging.getInstance().unsubscribeFromTopic("status").addOnSuccessListener(new OnSuccessListener<Void>() {
                        @Override
                        public void onSuccess(Void aVoid) {
                            SharedPreferences.Editor editor = getSharedPreferences(NOTIF, MODE_PRIVATE).edit();
                            editor.putBoolean(NOTIF, false);
                            editor.apply();
                            Toast.makeText(getApplicationContext(), "NOTIFIKASI MATI", Toast.LENGTH_SHORT).show();
                        }
                    });
                }
            }
        });

    }
}
