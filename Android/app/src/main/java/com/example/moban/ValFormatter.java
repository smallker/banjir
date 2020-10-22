package com.example.moban;

import com.github.mikephil.charting.components.AxisBase;
import com.github.mikephil.charting.data.Entry;
import com.github.mikephil.charting.formatter.ValueFormatter;

import java.text.DecimalFormat;

public class ValFormatter extends ValueFormatter {
    private DecimalFormat format = new DecimalFormat("###,##0.000");

    @Override
    public String getPointLabel(Entry entry) {
        return format.format(entry.getY());
    }

    @Override
    public String getAxisLabel(float value, AxisBase axis) {
        return format.format(value);
    }

}
