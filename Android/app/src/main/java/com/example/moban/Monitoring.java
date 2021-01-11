package com.example.moban;

import android.annotation.SuppressLint;
import android.content.Intent;
import android.os.Bundle;
import android.text.format.DateFormat;
import android.view.View;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseError;
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;
import com.google.firebase.database.ValueEventListener;

import java.math.BigDecimal;
import java.math.RoundingMode;
import java.sql.Timestamp;
import java.util.Calendar;
import java.util.Date;
import java.util.Locale;


public class Monitoring extends AppCompatActivity {
    TextView ketinggian, hujan, debit, status, txtjamtanggal, intensitas;
    DatabaseReference reff;
    private String TAG = "testing";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_monitoring);
        final Bundle bundle = new Bundle();
        final Intent intent = new Intent(this, Graph.class);
        getSupportActionBar().hide();
        ketinggian = findViewById(R.id.ketinggian);
        hujan = findViewById(R.id.hujan);
        debit = findViewById(R.id.debit);
        intensitas = findViewById(R.id.intensitas);
        status = findViewById(R.id.status);
        txtjamtanggal = findViewById(R.id.txtjamtanggal);
        reff = FirebaseDatabase.getInstance().getReference();
        findViewById(R.id.cardTinggi).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                bundle.putString(Graph.title, Graph.tinggi);
                intent.putExtras(bundle);
                startActivity(intent);
            }
        });
        findViewById(R.id.cardDebit).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                bundle.putString(Graph.title, Graph.debit);
                intent.putExtras(bundle);
                startActivity(intent);
            }
        });
        findViewById(R.id.cardhujan).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                bundle.putString(Graph.title, Graph.hujan);
                intent.putExtras(bundle);
                startActivity(intent);
            }
        });
        findViewById(R.id.cardIntensitas).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                bundle.putString(Graph.title, Graph.intensitas);
                intent.putExtras(bundle);
                startActivity(intent);
            }
        });
        reff.child("realtime").child("tembalang").addValueEventListener(new ValueEventListener() {
            @SuppressLint("SetTextI18n")
            @Override
            public void onDataChange(@NonNull DataSnapshot dataSnapshot) {
                try {
                    String banjir = round(dataSnapshot.child("tinggi").getValue(Double.class),3).toString() + " m";
                    String debitnya = round(dataSnapshot.child("debit").getValue(Double.class),3).toString() + " m3/s";
                    String hujann = round(dataSnapshot.child("hujan").getValue(Double.class),3).toString() + " mm";
                    String statusnya = dataSnapshot.child("status").getValue().toString();
                    Long timestamp = dataSnapshot.child("timestamp").getValue(Long.class);
                    String jamtanggal = getDate(timestamp);
                    String intensitasnya = round(dataSnapshot.child("intensitas").getValue(Double.class),3).toString();
                    ketinggian.setText(banjir);
                    debit.setText(debitnya);
                    hujan.setText(hujann);
                    status.setText(statusnya);
                    txtjamtanggal.setText("Data diambil pada: " + jamtanggal);
                    intensitas.setText(intensitasnya);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }

            @Override
            public void onCancelled(@NonNull DatabaseError databaseError) {

            }
        });
    }
    private static Double round(double value, int places) {
        if (places < 0) throw new IllegalArgumentException();

        BigDecimal bd = new BigDecimal(Double.toString(value));
        bd = bd.setScale(places, RoundingMode.HALF_UP);
        return bd.doubleValue();
    }
    private String getDate(long time) {
        Calendar cal = Calendar.getInstance(Locale.ENGLISH);
        cal.setTimeInMillis(time);
        String date = DateFormat.format("dd-MM-yyyy HH:mm", cal).toString();
        return date;
    }
}
