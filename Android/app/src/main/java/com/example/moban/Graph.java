package com.example.moban;

import android.annotation.SuppressLint;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.annotation.RequiresApi;
import androidx.appcompat.app.AppCompatActivity;

import com.github.mikephil.charting.charts.LineChart;
import com.github.mikephil.charting.data.Entry;
import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseError;
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;
import com.google.firebase.database.ValueEventListener;

import java.util.ArrayList;
import java.util.Objects;

public class Graph extends AppCompatActivity {
    private String TAG = "testing";
    public static String title = "title";
    public static String tinggi = "tinggi";
    public static String debit = "debit";
    public static String hujan = "hujan";
    public static String intensitas = "intensitas";
    private static int maxdata = 6;

    @RequiresApi(api = Build.VERSION_CODES.KITKAT)
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_graph);
        Objects.requireNonNull(getSupportActionBar()).hide();
        final Bundle data = getIntent().getExtras();
        final LineChart chart = findViewById(R.id.chart);
        final TextView tvTitle = findViewById(R.id.title);
        assert data != null;
        final String dataChoose = data.getString(Graph.title);
        DatabaseReference ref = FirebaseDatabase.getInstance().getReference("/sensor/tembalang/menit/");
        ref.limitToLast(maxdata).addValueEventListener(new ValueEventListener() {
            @SuppressLint("SetTextI18n")
            @Override
            public void onDataChange(@NonNull DataSnapshot snapshot) {
                int count = 0;
                String graphInfo = "";
                ArrayList<String> xVal = new ArrayList<>();
                ArrayList<Entry> yVal = new ArrayList<>();
                try {
                    for (DataSnapshot postSnapshot : snapshot.getChildren()) {
                        if(count == maxdata -1) graphInfo = postSnapshot.child("tanggal").getValue(String.class);
                        assert dataChoose != null;
                        Float value = postSnapshot.child(dataChoose).getValue(Float.class);
                        String[] _jam = postSnapshot.child("jam").getValue(String.class).split(":");
                        String jam = _jam[0] + "." + _jam[1];
                        xVal.add(jam);
                        yVal.add(new Entry(count, value));
                        count++;
                        Log.d(TAG, "onDataChange: "+value);
                    }
                    GraphSetting graph = new GraphSetting(chart, graphInfo, xVal, yVal);
                    graph.show();
                    tvTitle.setText("Grafik "+dataChoose);
                } catch (Exception e) {
                    Log.d(TAG, "onDataChange: ");
                    e.printStackTrace();
                }
            }

            @Override
            public void onCancelled(@NonNull DatabaseError error) {

            }
        });
    }
}