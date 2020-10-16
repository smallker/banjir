package com.example.moban;

import android.annotation.SuppressLint;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseError;
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;
import com.google.firebase.database.ValueEventListener;


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
                    String banjir = dataSnapshot.child("tinggi").getValue().toString() + " m";
                    String debitnya = dataSnapshot.child("debit").getValue().toString() + " m3/s";
                    String hujann = dataSnapshot.child("hujan").getValue().toString() + " mm";
                    String statusnya = dataSnapshot.child("status").getValue().toString();
                    String jamtanggal = dataSnapshot.child("jamtanggal").getValue().toString();
                    String intensitasnya = dataSnapshot.child("intensitas").getValue().toString();
                    ketinggian.setText(banjir);
                    debit.setText(debitnya);
                    hujan.setText(hujann);
                    status.setText(statusnya);
                    txtjamtanggal.setText("Data diambil pada: " + jamtanggal);
                    intensitas.setText(intensitasnya);
                } catch (Exception ignored) {
                }
            }

            @Override
            public void onCancelled(@NonNull DatabaseError databaseError) {

            }
        });
    }
}
