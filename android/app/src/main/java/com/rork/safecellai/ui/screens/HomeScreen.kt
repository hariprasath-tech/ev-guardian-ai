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
import androidx.compose.material.icons.filled.AcUnit
import androidx.compose.material.icons.filled.BatteryChargingFull
import androidx.compose.material.icons.filled.CheckCircle
import androidx.compose.material.icons.filled.Lock
import androidx.compose.material.icons.filled.Speed
import androidx.compose.material.icons.filled.Thermostat
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
fun HomeScreen(
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
            title = "SafeCell AI",
            pillLabel = "SafeCell X1 >",
            contentHeight = 240.dp
        ) {
            AsyncImage(
                model = ImageRequest.Builder(context)
                    .data(SafeCellAssets.HERO_CAR)
                    .crossfade(true)
                    .build(),
                contentDescription = "SafeCell X1 Vehicle",
                contentScale = ContentScale.Fit,
                modifier = Modifier.fillMaxSize()
            )
        }

        Spacer(modifier = Modifier.height(16.dp))

        CircularIconRow(
            actions = listOf(
                CircularAction(Icons.Filled.Lock, "Lock") {},
                CircularAction(Icons.Filled.AcUnit, "Climate") {},
                CircularAction(Icons.Filled.Speed, "Diagnostics") {}
            )
        )

        Spacer(modifier = Modifier.height(24.dp))

        BottomSheetPanel(
            headline = "Battery Health",
            headlineValue = "98",
            headlineValueSuffix = "%",
            headlineValueColor = MintGreen,
            subtext = "Fast charging",
            progress = 0.98f,
            progressColor = MintGreen,
            captionIcon = Icons.Filled.CheckCircle,
            captionText = "Healthy — last checked 2 min ago"
        ) {
            StatCardPair(
                card1 = StatCardData(
                    label = "Cycle Count",
                    value = "342",
                    unit = "",
                    icon = Icons.Filled.BatteryChargingFull
                ),
                card2 = StatCardData(
                    label = "Battery Temp",
                    value = "31",
                    unit = "°C",
                    icon = Icons.Filled.Thermostat
                )
            )
        }

        Spacer(modifier = Modifier.height(24.dp))
    }
}
