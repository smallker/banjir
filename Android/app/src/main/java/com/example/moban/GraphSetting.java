package com.example.moban;

import android.graphics.Color;

import com.github.mikephil.charting.charts.LineChart;
import com.github.mikephil.charting.components.Description;
import com.github.mikephil.charting.components.XAxis;
import com.github.mikephil.charting.components.YAxis;
import com.github.mikephil.charting.data.Entry;
import com.github.mikephil.charting.data.LineData;
import com.github.mikephil.charting.data.LineDataSet;
import com.github.mikephil.charting.formatter.IndexAxisValueFormatter;
import com.github.mikephil.charting.interfaces.datasets.ILineDataSet;

import java.util.ArrayList;

public class GraphSetting {
    private final LineChart chart;
    private final String title;
    private final ArrayList<String> xVal;
    private final ArrayList<Entry> yVal;

    public GraphSetting(LineChart chart, String title, ArrayList<String>xVal, ArrayList<Entry>yVal){
        this.chart = chart;
        this.title = title;
        this.xVal = xVal;
        this.yVal = yVal;
    }
    public void show(){
        Description description = new Description();
        description.setText("");
        chart.setDescription(description);
        chart.setDragEnabled(true);
        chart.setScaleEnabled(true);
        chart.setDrawBorders(false);
        chart.setBackgroundColor(Color.WHITE);
        chart.getAxisRight().setEnabled(false);
        chart.getXAxis().setDrawGridLines(false);
        chart.getAxisLeft().setDrawGridLines(false);
        chart.getAxisRight().setDrawGridLines(false);
        chart.getAxisLeft().setEnabled(false);
        chart.getCenterOffsets();
        XAxis xAxis = chart.getXAxis();
        xAxis.setAxisLineWidth(4);
        xAxis.setGranularity(1f);
        xAxis.setValueFormatter(new IndexAxisValueFormatter(xVal));
        xAxis.setPosition(XAxis.XAxisPosition.BOTTOM);
        LineDataSet dataSet = new LineDataSet(yVal,title);
        dataSet.setMode(LineDataSet.Mode.CUBIC_BEZIER);
        dataSet.setValueFormatter(new ValFormatter());
        dataSet.setFillAlpha(110);
        dataSet.setCircleRadius(10f);
        dataSet.setDrawCircles(true);
        dataSet.setDrawFilled(true);
        ArrayList<ILineDataSet>iLineDataSets = new ArrayList<>();
        iLineDataSets.add(dataSet);
        LineData data = new LineData(iLineDataSets);
        chart.setData(data);
        chart.invalidate();
    }
}
