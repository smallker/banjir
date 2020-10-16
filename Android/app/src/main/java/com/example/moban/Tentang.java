package com.example.moban;

import androidx.appcompat.app.AppCompatActivity;
import androidx.cardview.widget.CardView;

import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.view.View;

public class Tentang extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_tentang);
        getSupportActionBar().hide();

        ((CardView) findViewById(R.id.cardwebsite)).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent website = new Intent();
                website.setAction(Intent.ACTION_VIEW);
                website.addCategory(Intent.CATEGORY_BROWSABLE);
                website.setData(Uri.parse("https://www.moban.web.id/"));
                startActivity(website);
            }
        });
    }
}
