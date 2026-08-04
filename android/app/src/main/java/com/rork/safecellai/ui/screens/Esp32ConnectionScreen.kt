package com.rork.safecellai.ui.screens

import androidx.compose.foundation.BorderStroke
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Bluetooth
import androidx.compose.material.icons.filled.ContentCopy
import androidx.compose.material.icons.filled.Devices
import androidx.compose.material.icons.filled.Memory
import androidx.compose.material.icons.filled.PhoneAndroid
import androidx.compose.material.icons.filled.Refresh
import androidx.compose.material.icons.filled.Wifi
import androidx.compose.material3.Icon
import androidx.compose.material3.Surface
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.layout.ContentScale
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.rork.safecellai.ui.widgets.BottomSheetPanel
import com.rork.safecellai.ui.widgets.CircularAction
import com.rork.safecellai.ui.widgets.CircularIconRow
import com.rork.safecellai.ui.widgets.HeroGradientHeader
import com.rork.safecellai.ui.widgets.StatCardData
import com.rork.safecellai.ui.widgets.StatCardPair
import com.rork.safecellai.SafeCellAssets
import com.rork.safecellai.ui.theme.HairlineGray
import com.rork.safecellai.ui.theme.MintGreen
import com.rork.safecellai.ui.theme.MutedGray
import com.rork.safecellai.ui.theme.NearBlack
import com.rork.safecellai.ui.theme.Poppins
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.PaddingValues
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.verticalScroll
import coil3.compose.AsyncImage
import coil3.request.ImageRequest
import coil3.request.crossfade

@Composable
fun Esp32ConnectionScreen(
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
            title = "ESP32 Connection",
            pillLabel = "SafeCell Node 01 >",
            contentHeight = 240.dp
        ) {
            AsyncImage(
                model = ImageRequest.Builder(context)
                    .data(SafeCellAssets.ESP32_CHIP)
                    .crossfade(true)
                    .build(),
                contentDescription = "ESP32 Chip",
                contentScale = ContentScale.Fit,
                modifier = Modifier.fillMaxSize()
            )
        }

        Spacer(modifier = Modifier.height(16.dp))

        CircularIconRow(
            actions = listOf(
                CircularAction(Icons.Filled.Refresh, "Scan") {},
                CircularAction(Icons.Filled.PhoneAndroid, "Pair") {},
                CircularAction(Icons.Filled.Devices, "Forget") {}
            )
        )

        Spacer(modifier = Modifier.height(24.dp))

        BottomSheetPanel(
            headline = "Connection",
            headlineValue = "Connected",
            headlineValueColor = MintGreen,
            subtext = "Signal strength: Excellent",
            progress = 0.82f,
            progressColor = MintGreen,
            captionIcon = Icons.Filled.Bluetooth,
            captionText = "Device ID: SC-ESP32-04A2",
            extraContent = { MacAddressRow() }
        ) {
            StatCardPair(
                card1 = StatCardData(
                    label = "Last Synced",
                    value = "Just now",
                    unit = "",
                    icon = Icons.Filled.Refresh
                ),
                card2 = StatCardData(
                    label = "Firmware",
                    value = "v2.3.1",
                    unit = "",
                    icon = Icons.Filled.Memory
                )
            )
        }

        Spacer(modifier = Modifier.height(24.dp))
    }
}

@Composable
private fun MacAddressRow() {
    Surface(
        modifier = Modifier
            .fillMaxWidth()
            .clip(RoundedCornerShape(14.dp)),
        shape = RoundedCornerShape(14.dp),
        color = Color(0xFFFAFBFC),
        border = BorderStroke(1.dp, HairlineGray)
    ) {
        Row(
            modifier = Modifier
                .fillMaxWidth()
                .padding(horizontal = 16.dp, vertical = 14.dp),
            verticalAlignment = Alignment.CenterVertically
        ) {
            Icon(
                imageVector = Icons.Filled.Wifi,
                contentDescription = null,
                tint = MutedGray,
                modifier = Modifier.size(18.dp)
            )
            Spacer(modifier = Modifier.width(12.dp))
            Text(
                text = "MAC Address",
                fontSize = 13.sp,
                fontWeight = FontWeight.Medium,
                fontFamily = Poppins,
                color = MutedGray
            )
            Spacer(modifier = Modifier.weight(1f))
            Text(
                text = "A4:CF:12:8B:3E:9D",
                fontSize = 13.sp,
                fontWeight = FontWeight.Medium,
                fontFamily = FontFamily.Monospace,
                color = NearBlack
            )
            Spacer(modifier = Modifier.width(8.dp))
            Icon(
                imageVector = Icons.Filled.ContentCopy,
                contentDescription = "Copy",
                tint = MutedGray,
                modifier = Modifier.size(16.dp)
            )
        }
    }
}
