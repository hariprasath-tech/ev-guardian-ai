package com.rork.safecellai.ui.screens

import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.interaction.MutableInteractionSource
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
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Build
import androidx.compose.material.icons.filled.History
import androidx.compose.material.icons.filled.Lock
import androidx.compose.material.icons.filled.LockOpen
import androidx.compose.material.icons.filled.Science
import androidx.compose.material.icons.filled.Verified
import androidx.compose.material3.Icon
import androidx.compose.material3.Surface
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableFloatStateOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.Color
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
import com.rork.safecellai.ui.theme.NearBlack
import com.rork.safecellai.ui.theme.Poppins
import com.rork.safecellai.ui.theme.White
import com.rork.safecellai.ui.widgets.AnimatedProgressBar
import com.rork.safecellai.ui.widgets.CircularAction
import com.rork.safecellai.ui.widgets.CircularIconRow
import com.rork.safecellai.ui.widgets.HeroGradientHeader
import com.rork.safecellai.ui.widgets.SliderRow
import com.rork.safecellai.ui.widgets.StatCardData
import com.rork.safecellai.ui.widgets.StatCardPair

@Composable
fun FireSuppressionScreen(
    contentPadding: PaddingValues = PaddingValues(0.dp)
) {
    val scrollState = rememberScrollState()
    val context = LocalContext.current
    var systemArmed by remember { mutableStateOf(true) }
    var sensitivity by remember { mutableFloatStateOf(0.6f) }

    Column(
        modifier = Modifier
            .fillMaxSize()
            .verticalScroll(scrollState)
            .padding(bottom = contentPadding.calculateBottomPadding())
    ) {
        HeroGradientHeader(
            title = "Fire Suppression",
            pillLabel = "Suppression System >",
            contentHeight = 240.dp
        ) {
            AsyncImage(
                model = ImageRequest.Builder(context)
                    .data(SafeCellAssets.SUPPRESSION)
                    .crossfade(true)
                    .build(),
                contentDescription = "Suppression System Diagram",
                contentScale = ContentScale.Fit,
                modifier = Modifier.fillMaxSize()
            )
        }

        Spacer(modifier = Modifier.height(16.dp))

        CircularIconRow(
            actions = listOf(
                CircularAction(
                    if (systemArmed) Icons.Filled.Lock else Icons.Filled.LockOpen,
                    if (systemArmed) "Disarm" else "Arm"
                ) { systemArmed = !systemArmed },
                CircularAction(Icons.Filled.Science, "Test") {},
                CircularAction(Icons.Filled.History, "Log") {}
            )
        )

        Spacer(modifier = Modifier.height(24.dp))

        SuppressionBottomSheet(
            systemArmed = systemArmed,
            onToggleArmed = { systemArmed = !systemArmed },
            sensitivity = sensitivity,
            onSensitivityChange = { sensitivity = it }
        )

        Spacer(modifier = Modifier.height(24.dp))
    }
}

@Composable
private fun SuppressionBottomSheet(
    systemArmed: Boolean,
    onToggleArmed: () -> Unit,
    sensitivity: Float,
    onSensitivityChange: (Float) -> Unit
) {
    val headline = if (systemArmed) "System Armed" else "Disarmed"
    val headlineColor = if (systemArmed) MintGreen else MutedGray
    val subtext = if (systemArmed) "All nozzles ready" else "System inactive"
    val progress = 0.94f
    val progressColor = if (systemArmed) MintGreen else MutedGray

    Surface(
        modifier = Modifier.fillMaxWidth(),
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
                modifier = Modifier
                    .fillMaxWidth()
                    .clip(RoundedCornerShape(16.dp))
                    .background(if (systemArmed) Color(0xFFE8F8EE) else Color(0xFFF5F6F8))
                    .clickable(
                        interactionSource = remember { MutableInteractionSource() },
                        indication = null
                    ) { onToggleArmed() }
                    .padding(horizontal = 16.dp, vertical = 12.dp),
                horizontalArrangement = Arrangement.SpaceBetween,
                verticalAlignment = Alignment.CenterVertically
            ) {
                Row(
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Box(
                        modifier = Modifier
                            .size(10.dp)
                            .clip(CircleShape)
                            .background(headlineColor)
                    )
                    Spacer(modifier = Modifier.width(10.dp))
                    Text(
                        text = headline,
                        fontSize = 20.sp,
                        fontWeight = FontWeight.SemiBold,
                        fontFamily = Poppins,
                        color = headlineColor
                    )
                }
                Surface(
                    shape = RoundedCornerShape(50),
                    color = if (systemArmed) MintGreen else HairlineGray,
                    modifier = Modifier.size(width = 52.dp, height = 28.dp)
                ) {
                    Box(
                        modifier = Modifier.fillMaxSize(),
                        contentAlignment = Alignment.CenterStart
                    ) {
                        Box(
                            modifier = Modifier
                                .size(24.dp)
                                .clip(CircleShape)
                                .background(White)
                                .graphicsLayer {
                                    translationX = if (systemArmed) 24.dp.toPx() else 2.dp.toPx()
                                }
                        )
                    }
                }
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

            Text(
                text = "Agent Remaining",
                fontSize = 12.sp,
                fontWeight = FontWeight.Medium,
                fontFamily = Poppins,
                color = MutedGray
            )
            Spacer(modifier = Modifier.height(6.dp))
            AnimatedProgressBar(
                progress = progress,
                barColor = progressColor
            )
            Spacer(modifier = Modifier.height(10.dp))

            Row(
                verticalAlignment = Alignment.CenterVertically
            ) {
                Icon(
                    imageVector = Icons.Filled.History,
                    contentDescription = null,
                    tint = MutedGray,
                    modifier = Modifier.size(15.dp)
                )
                Spacer(modifier = Modifier.width(6.dp))
                Text(
                    text = "Last triggered: Never",
                    fontSize = 12.sp,
                    fontWeight = FontWeight.Normal,
                    fontFamily = Poppins,
                    color = MutedGray
                )
            }

            Spacer(modifier = Modifier.height(20.dp))

            Text(
                text = "Sensitivity Threshold",
                fontSize = 14.sp,
                fontWeight = FontWeight.SemiBold,
                fontFamily = Poppins,
                color = NearBlack
            )
            Spacer(modifier = Modifier.height(4.dp))
            Text(
                text = "${(sensitivity * 100).toInt()}% detection sensitivity",
                fontSize = 12.sp,
                fontWeight = FontWeight.Normal,
                fontFamily = Poppins,
                color = MutedGray
            )
            Spacer(modifier = Modifier.height(12.dp))

            SliderRow(
                minValue = 0f,
                maxValue = 1f,
                currentValue = sensitivity,
                onValueChange = onSensitivityChange,
                trackGradient = listOf(MintGreen, AlertRed)
            )

            Spacer(modifier = Modifier.height(20.dp))

            StatCardPair(
                card1 = StatCardData(
                    label = "Agent Remaining",
                    value = "94",
                    unit = "%",
                    icon = Icons.Filled.Verified
                ),
                card2 = StatCardData(
                    label = "Last Test",
                    value = "3 days",
                    unit = " ago",
                    icon = Icons.Filled.Build
                )
            )
            Spacer(modifier = Modifier.height(16.dp))
        }
    }
}
