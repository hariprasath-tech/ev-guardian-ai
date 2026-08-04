package com.rork.safecellai.ui.screens

import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.PaddingValues
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Air
import androidx.compose.material.icons.filled.Refresh
import androidx.compose.material.icons.filled.History
import androidx.compose.material.icons.filled.LocalFireDepartment
import androidx.compose.material.icons.filled.Tune
import androidx.compose.material.icons.filled.Wifi
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.layout.ContentScale
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.unit.dp
import coil3.compose.AsyncImage
import coil3.request.ImageRequest
import coil3.request.crossfade
import com.rork.safecellai.SafeCellAssets
import com.rork.safecellai.ui.theme.MintGreen
import com.rork.safecellai.ui.widgets.BottomSheetPanel
import com.rork.safecellai.ui.widgets.CircularAction
import com.rork.safecellai.ui.widgets.CircularIconRow
import com.rork.safecellai.ui.widgets.HeroGradientHeader
import com.rork.safecellai.ui.widgets.StatCardData
import com.rork.safecellai.ui.widgets.StatCardPair

@Composable
fun GasMonitoringScreen(
    contentPadding: PaddingValues = PaddingValues(0.dp)
) {
    val scrollState = rememberScrollState()
    val context = LocalContext.current

    Column(
        modifier = Modifier
            .fillMaxSize()
            .verticalScroll(scrollState)
            .padding(bottom = contentPadding.calculateBottomPadding())
    ) {
        HeroGradientHeader(
            title = "Gas Monitoring",
            pillLabel = "Cabin Sensors >",
            contentHeight = 240.dp
        ) {
            AsyncImage(
                model = ImageRequest.Builder(context)
                    .data(SafeCellAssets.CABIN_SENSOR)
                    .crossfade(true)
                    .build(),
                contentDescription = "Cabin Sensor Diagram",
                contentScale = ContentScale.Fit,
                modifier = Modifier.fillMaxSize()
            )
        }

        Spacer(modifier = Modifier.height(16.dp))

        CircularIconRow(
            actions = listOf(
                CircularAction(Icons.Filled.Refresh, "Refresh") {},
                CircularAction(Icons.Filled.History, "Sensor Log") {},
                CircularAction(Icons.Filled.Tune, "Calibrate") {}
            )
        )

        Spacer(modifier = Modifier.height(24.dp))

        BottomSheetPanel(
            headline = "Air Quality",
            headlineValue = "Good",
            headlineValueColor = MintGreen,
            subtext = "All sensors within normal range",
            progress = 0.35f,
            progressColor = MintGreen,
            captionIcon = Icons.Filled.Wifi,
            captionText = "Updated 5 seconds ago"
        ) {
            StatCardPair(
                card1 = StatCardData(
                    label = "CO₂ Level",
                    value = "412",
                    unit = " ppm",
                    icon = Icons.Filled.Air
                ),
                card2 = StatCardData(
                    label = "Smoke Density",
                    value = "0.02",
                    unit = " mg/m³",
                    icon = Icons.Filled.LocalFireDepartment
                )
            )
        }

        Spacer(modifier = Modifier.height(24.dp))
    }
}
