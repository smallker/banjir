package com.example.moban;

import androidx.appcompat.app.AppCompatActivity;
import androidx.cardview.widget.CardView;

import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.view.View;

public class Data extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_data);
        getSupportActionBar().hide();

        ((CardView) findViewById(R.id.cardkalat)).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent web = new Intent();
                web.setAction(Intent.ACTION_VIEW);
                web.addCategory(Intent.CATEGORY_BROWSABLE);
                web.setData(Uri.parse("https://www.moban.web.id/about#carakerja/"));
                startActivity(web);
            }
        });
    }
}
