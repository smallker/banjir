package com.example.moban;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseError;
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;
import com.google.firebase.database.ValueEventListener;


public class Monitoring extends AppCompatActivity {
    TextView ketinggian,hujan ,debit,status, txtjamtanggal;
    Button btn;
    DatabaseReference reff;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_monitoring);
        getSupportActionBar().hide();
        ketinggian = findViewById(R.id.ketinggian);
        hujan = findViewById(R.id.hujan);
        debit = findViewById(R.id.debit);
        status = findViewById(R.id.status);
        txtjamtanggal = findViewById(R.id.txtjamtanggal);

        reff = FirebaseDatabase.getInstance().getReference();
        reff.child("realtime").child("tembalang").addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(@NonNull DataSnapshot dataSnapshot) {
                String banjir=dataSnapshot.child("tinggi").getValue().toString();
                String debitnya=dataSnapshot.child("debit").getValue().toString();
                String hujann=dataSnapshot.child("hujan").getValue().toString();
                String statusnya=dataSnapshot.child("status").getValue().toString();
                String jamtanggal=dataSnapshot.child("jamtanggal").getValue().toString();

                ketinggian.setText(banjir);
                debit.setText(debitnya);
                hujan.setText(hujann);
                status.setText(statusnya);
                txtjamtanggal.setText("Data diambil pada: " + jamtanggal);
//                jamtanggal = "Data diambil pada : " + jamtanggal;
//
//                Toast.makeText(Monitoring.this, jamtanggal, Toast.LENGTH_LONG).show();

            }

            @Override
            public void onCancelled(@NonNull DatabaseError databaseError) {

            }
        });
    }
}
