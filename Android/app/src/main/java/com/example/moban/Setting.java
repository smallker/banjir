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

    private SharedPreferences notifSet;
    private static final String NOTIF = "Notif";
    private SwitchCompat tb_notif;
    TextView txtKetNotif;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_setting);
        getSupportActionBar().hide();
        tb_notif = findViewById(R.id.btn_notif);
        txtKetNotif = findViewById(R.id.txt_ket_notif);
        notifSet = getSharedPreferences(NOTIF, Context.MODE_PRIVATE);
        tb_notif.setChecked(notifSet.getBoolean(NOTIF, false));
        tb_notif.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if (isChecked) {
                    txtKetNotif.setText("Anda Akan Menerima Notifikasi Ketika Pergantian Status");
                    FirebaseMessaging.getInstance().subscribeToTopic("status").addOnSuccessListener(new OnSuccessListener<Void>() {
                        @Override
                        public void onSuccess(Void aVoid) {
                            //Untuk mengaktifkan kondisi state ON
                            SharedPreferences.Editor editor = getSharedPreferences(NOTIF, MODE_PRIVATE).edit();
                            editor.putBoolean(NOTIF, true);
                            editor.apply();
                            Toast.makeText(getApplicationContext(), "NOTIFIKASI HIDUP", Toast.LENGTH_SHORT).show();
                        }
                    });
                } else {
                    txtKetNotif.setText("Anda Tidak Akan Menerima Notifikasi Ketika Pergantian Status");
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
