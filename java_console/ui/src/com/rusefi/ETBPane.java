package com.rusefi;

import com.rusefi.core.Sensor;
import com.rusefi.ui.GaugesGridElement;
import com.rusefi.ui.etb.CalibrationPanel;
import com.rusefi.ui.etb.CommandsPanel;
import com.rusefi.ui.util.UiUtils;

import javax.swing.*;
import java.awt.*;

/**
 * Controls related to Electronic Throttle Body
 *
 * (c) Andrey Belomutskiy 2013-2019
 */
public class ETBPane {
    public static final String SET_ETB = "set_etb ";
    private final JPanel content = new JPanel(new BorderLayout());

    public ETBPane() {
        JPanel centerPanel = new JPanel(new GridLayout(3, 1));
        centerPanel.add(GaugesGridElement.create(Sensor.PPS));
        centerPanel.add(GaugesGridElement.create(Sensor.ETB_CONTROL_QUALITY));
        centerPanel.add(GaugesGridElement.create(Sensor.TPS));

        content.add(new CommandsPanel().getContent(), BorderLayout.WEST);
        content.add(centerPanel, BorderLayout.CENTER);
        content.add(new CalibrationPanel().getContent(), BorderLayout.EAST);
    }

    public JPanel getContent() {
        return content;
    }

}
