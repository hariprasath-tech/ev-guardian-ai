package com.rork.safecellai.ui.screens

import androidx.compose.animation.core.RepeatMode
import androidx.compose.animation.core.animateFloat
import androidx.compose.animation.core.infiniteRepeatable
import androidx.compose.animation.core.rememberInfiniteTransition
import androidx.compose.animation.core.tween
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.PaddingValues
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Call
import androidx.compose.material.icons.filled.CheckCircle
import androidx.compose.material.icons.filled.Contacts
import androidx.compose.material.icons.filled.LocalFireDepartment
import androidx.compose.material.icons.filled.Thermostat
import androidx.compose.material.icons.filled.Warning
import androidx.compose.material3.Icon
import androidx.compose.material3.Surface
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.graphicsLayer
import androidx.compose.ui.layout.ContentScale
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import coil3.compose.AsyncImage
import coil3.request.ImageRequest
import coil3.request.crossfade
import com.rork.safecellai.SafeCellAssets
import com.rork.safecellai.ui.theme.AlertRed
import com.rork.safecellai.ui.theme.HairlineGray
import com.rork.safecellai.ui.theme.MintGreen
import com.rork.safecellai.ui.theme.MutedGray
import com.rork.safecellai.ui.theme.Poppins
import com.rork.safecellai.ui.theme.White
import com.rork.safecellai.ui.widgets.AnimatedProgressBar
import com.rork.safecellai.ui.widgets.CircularAction
import com.rork.safecellai.ui.widgets.CircularIconRow
import com.rork.safecellai.ui.widgets.HeroGradientHeader
import com.rork.safecellai.ui.widgets.StatCardData
import com.rork.safecellai.ui.widgets.StatCardPair

@Composable
fun EmergencyScreen(
    contentPadding: PaddingValues = PaddingValues(0.dp)
) {
    val scrollState = rememberScrollState()
    val context = LocalContext.current
    val alertActive = false

    Column(
        modifier = Modifier
            .fillMaxSize()
            .verticalScroll(scrollState)
            .padding(bottom = contentPadding.calculateBottomPadding())
    ) {
        HeroGradientHeader(
            title = "Emergency",
            pillLabel = "Emergency Status >",
            contentHeight = 240.dp,
            alertTone = alertActive
        ) {
            AsyncImage(
                model = ImageRequest.Builder(context)
                    .data(SafeCellAssets.HERO_CAR)
                    .crossfade(true)
                    .build(),
                contentDescription = "Vehicle Emergency View",
                contentScale = ContentScale.Fit,
                modifier = Modifier.fillMaxSize()
            )
        }

        Spacer(modifier = Modifier.height(16.dp))

        CircularIconRow(
            actions = listOf(
                CircularAction(Icons.Filled.Call, "Call 911") {},
                CircularAction(Icons.Filled.Contacts, "Notify") {},
                CircularAction(Icons.Filled.Warning, "Cancel", enabled = alertActive) {}
            )
        )

        Spacer(modifier = Modifier.height(24.dp))

        EmergencyBottomSheet(alertActive = alertActive)

        Spacer(modifier = Modifier.height(24.dp))
    }
}

@Composable
private fun EmergencyBottomSheet(
    alertActive: Boolean
) {
    val headline = if (alertActive) "ALERT ACTIVE" else "All Systems Normal"
    val headlineColor = if (alertActive) AlertRed else MintGreen
    val subtext = if (alertActive) "Thermal anomaly detected" else "No anomalies detected"
    val progress = if (alertActive) 0.27f else 0f
    val progressColor = if (alertActive) AlertRed else MintGreen
    val captionIcon = if (alertActive) Icons.Filled.Warning else Icons.Filled.CheckCircle
    val captionText = if (alertActive) "Auto-response in 8s" else "System monitored continuously"

    val infiniteTransition = rememberInfiniteTransition(label = "pulse")
    val pulseAlpha by infiniteTransition.animateFloat(
        initialValue = 1f,
        targetValue = if (alertActive) 0.4f else 1f,
        animationSpec = infiniteRepeatable(
            animation = tween(800),
            repeatMode = RepeatMode.Reverse
        ),
        label = "pulseAlpha"
    )

    Surface(
        modifier = Modifier
            .fillMaxWidth()
            .graphicsLayer { alpha = pulseAlpha },
        shape = RoundedCornerShape(topStart = 28.dp, topEnd = 28.dp),
        color = White,
        shadowElevation = 20.dp
    ) {
        Column(
            modifier = Modifier
                .fillMaxWidth()
                .padding(horizontal = 24.dp, vertical = 16.dp)
        ) {
            Box(
                modifier = Modifier
                    .width(40.dp)
                    .height(5.dp)
                    .clip(RoundedCornerShape(3.dp))
                    .background(HairlineGray)
                    .align(Alignment.CenterHorizontally)
            )
            Spacer(modifier = Modifier.height(20.dp))

            Row(
                modifier = Modifier.fillMaxWidth(),
                horizontalArrangement = Arrangement.SpaceBetween,
                verticalAlignment = Alignment.Bottom
            ) {
                Text(
                    text = headline,
                    fontSize = if (alertActive) 22.sp else 20.sp,
                    fontWeight = FontWeight.SemiBold,
                    fontFamily = Poppins,
                    color = headlineColor
                )
            }
            Spacer(modifier = Modifier.height(4.dp))
            Text(
                text = subtext,
                fontSize = 13.sp,
                fontWeight = FontWeight.Normal,
                fontFamily = Poppins,
                color = MutedGray
            )
            Spacer(modifier = Modifier.height(14.dp))

            AnimatedProgressBar(
                progress = progress,
                barColor = progressColor
            )
            Spacer(modifier = Modifier.height(10.dp))

            Row(
                verticalAlignment = Alignment.CenterVertically
            ) {
                Icon(
                    imageVector = captionIcon,
                    contentDescription = null,
                    tint = if (alertActive) AlertRed else MutedGray,
                    modifier = Modifier.size(15.dp)
                )
                Spacer(modifier = Modifier.width(6.dp))
                Text(
                    text = captionText,
                    fontSize = 12.sp,
                    fontWeight = FontWeight.Normal,
                    fontFamily = Poppins,
                    color = if (alertActive) AlertRed else MutedGray
                )
            }

            Spacer(modifier = Modifier.height(18.dp))

            StatCardPair(
                card1 = StatCardData(
                    label = "Battery Temp",
                    value = "31",
                    unit = "°C",
                    icon = Icons.Filled.Thermostat,
                    valueColor = MintGreen
                ),
                card2 = StatCardData(
                    label = "Flame Sensor",
                    value = "Clear",
                    unit = "",
                    icon = Icons.Filled.LocalFireDepartment,
                    valueColor = MintGreen
                )
            )
            Spacer(modifier = Modifier.height(16.dp))
        }
    }
}
