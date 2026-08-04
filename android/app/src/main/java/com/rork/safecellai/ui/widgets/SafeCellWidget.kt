package com.rork.safecellai.ui.widgets

import androidx.compose.animation.core.FastOutSlowInEasing
import androidx.compose.animation.core.Spring
import androidx.compose.animation.core.animateFloatAsState
import androidx.compose.animation.core.spring
import androidx.compose.animation.core.tween
import androidx.compose.foundation.BorderStroke
import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.interaction.MutableInteractionSource
import androidx.compose.foundation.interaction.collectIsPressedAsState
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.automirrored.filled.KeyboardArrowRight
import androidx.compose.material.icons.filled.ContentCopy
import androidx.compose.material.icons.filled.Notifications
import androidx.compose.material3.Card
import androidx.compose.material3.CardDefaults
import androidx.compose.material3.Icon
import androidx.compose.material3.IconButton
import androidx.compose.material3.Surface
import androidx.compose.material3.Switch
import androidx.compose.material3.SwitchDefaults
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableFloatStateOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.draw.shadow
import androidx.compose.ui.graphics.Brush
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.graphicsLayer
import androidx.compose.ui.graphics.vector.ImageVector
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.Dp
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.rork.safecellai.ui.theme.AlertRed
import com.rork.safecellai.ui.theme.DarkGray
import com.rork.safecellai.ui.theme.HairlineGray
import com.rork.safecellai.ui.theme.MintGreen
import com.rork.safecellai.ui.theme.MutedGray
import com.rork.safecellai.ui.theme.NearBlack
import com.rork.safecellai.ui.theme.Poppins
import com.rork.safecellai.ui.theme.SoftGray
import com.rork.safecellai.ui.theme.White
import kotlinx.coroutines.delay

@Composable
fun CountUpText(
    targetValue: Float,
    modifier: Modifier = Modifier,
    color: Color = NearBlack,
    fontSize: Int = 34,
    fontWeight: FontWeight = FontWeight.Bold,
    decimals: Int = 0,
    prefix: String = "",
    suffix: String = ""
) {
    var animatedValue by remember { mutableFloatStateOf(0f) }
    LaunchedEffect(targetValue) {
        delay(150)
        val steps = 40
        val stepDuration = 800 / steps
        repeat(steps + 1) { step ->
            animatedValue = (targetValue * step / steps)
            delay(stepDuration.toLong())
        }
        animatedValue = targetValue
    }
    val displayValue = if (decimals > 0) {
        String.format("%.${decimals}f", animatedValue)
    } else {
        animatedValue.toInt().toString()
    }
    Text(
        text = "$prefix$displayValue$suffix",
        color = color,
        fontSize = fontSize.sp,
        fontWeight = fontWeight,
        fontFamily = Poppins,
        modifier = modifier
    )
}

@Composable
fun AnimatedProgressBar(
    progress: Float,
    modifier: Modifier = Modifier,
    barColor: Color = MintGreen,
    trackColor: Color = HairlineGray,
    barHeight: Dp = 10.dp,
    cornerRadius: Int = 8
) {
    var target by remember { mutableFloatStateOf(0f) }
    LaunchedEffect(progress) {
        delay(300)
        target = progress
    }
    val animatedProgress by animateFloatAsState(
        targetValue = target,
        animationSpec = tween(1000, easing = FastOutSlowInEasing),
        label = "progressFill"
    )
    Box(
        modifier = modifier
            .fillMaxWidth()
            .height(barHeight)
            .clip(RoundedCornerShape(cornerRadius.dp))
            .background(trackColor)
    ) {
        Box(
            modifier = Modifier
                .fillMaxWidth(animatedProgress)
                .height(barHeight)
                .clip(RoundedCornerShape(cornerRadius.dp))
                .background(barColor)
        )
    }
}

@Composable
fun HeroGradientHeader(
    title: String,
    pillLabel: String,
    modifier: Modifier = Modifier,
    alertTone: Boolean = false,
    contentHeight: Dp = 260.dp,
    heroContent: @Composable () -> Unit
) {
    val gradientColors = if (alertTone) {
        listOf(Color(0xFFF5E6E5), Color(0xFFF5F6F8))
    } else {
        listOf(Color(0xFFECEEF1), SoftGray)
    }
    Box(
        modifier = modifier
            .fillMaxWidth()
            .height(contentHeight + 80.dp)
            .background(Brush.verticalGradient(gradientColors))
    ) {
        Column(
            modifier = Modifier
                .fillMaxWidth()
                .padding(top = 16.dp, start = 20.dp, end = 20.dp),
            horizontalAlignment = Alignment.CenterHorizontally
        ) {
            Row(
                modifier = Modifier.fillMaxWidth(),
                horizontalArrangement = Arrangement.SpaceBetween,
                verticalAlignment = Alignment.CenterVertically
            ) {
                Text(
                    text = title,
                    fontSize = 22.sp,
                    fontWeight = FontWeight.SemiBold,
                    fontFamily = Poppins,
                    color = NearBlack
                )
                IconButton(onClick = {}) {
                    Icon(
                        imageVector = Icons.Filled.Notifications,
                        contentDescription = "Notifications",
                        tint = DarkGray,
                        modifier = Modifier.size(24.dp)
                    )
                }
            }
            Spacer(modifier = Modifier.height(8.dp))
            Surface(
                shape = RoundedCornerShape(50),
                color = Color(0xFFF0F1F3),
                border = BorderStroke(1.dp, HairlineGray),
                modifier = Modifier.clickable(
                    interactionSource = remember { MutableInteractionSource() },
                    indication = null
                ) {}
            ) {
                Row(
                    modifier = Modifier.padding(horizontal = 18.dp, vertical = 8.dp),
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Text(
                        text = pillLabel,
                        fontSize = 13.sp,
                        fontWeight = FontWeight.Medium,
                        fontFamily = Poppins,
                        color = DarkGray
                    )
                    Spacer(modifier = Modifier.width(4.dp))
                    Icon(
                        imageVector = Icons.AutoMirrored.Filled.KeyboardArrowRight,
                        contentDescription = "Select",
                        tint = MutedGray,
                        modifier = Modifier.size(18.dp)
                    )
                }
            }
            Spacer(modifier = Modifier.height(12.dp))
            var heroVisible by remember { mutableStateOf(false) }
            LaunchedEffect(Unit) {
                delay(200)
                heroVisible = true
            }
            val heroScale by animateFloatAsState(
                targetValue = if (heroVisible) 1f else 0.7f,
                animationSpec = spring(
                    dampingRatio = Spring.DampingRatioMediumBouncy,
                    stiffness = Spring.StiffnessMediumLow
                ),
                label = "heroScale"
            )
            val heroAlpha by animateFloatAsState(
                targetValue = if (heroVisible) 1f else 0f,
                animationSpec = tween(600),
                label = "heroAlpha"
            )
            Box(
                modifier = Modifier
                    .size(contentHeight)
                    .graphicsLayer {
                        scaleX = heroScale
                        scaleY = heroScale
                        alpha = heroAlpha
                    },
                contentAlignment = Alignment.Center
            ) {
                heroContent()
            }
        }
    }
}

@Composable
fun CircularIconButton(
    icon: ImageVector,
    label: String,
    onClick: () -> Unit,
    modifier: Modifier = Modifier,
    enabled: Boolean = true,
    iconTint: Color = NearBlack
) {
    val interactionSource = remember { MutableInteractionSource() }
    val isPressed by interactionSource.collectIsPressedAsState()
    val scale by animateFloatAsState(
        targetValue = if (isPressed) 0.85f else 1f,
        animationSpec = spring(
            dampingRatio = Spring.DampingRatioMediumBouncy,
            stiffness = Spring.StiffnessMedium
        ),
        label = "scale"
    )
    Column(
        modifier = modifier
            .clickable(
                interactionSource = interactionSource,
                indication = null,
                enabled = enabled
            ) { onClick() },
        horizontalAlignment = Alignment.CenterHorizontally
    ) {
        Surface(
            shape = CircleShape,
            color = if (enabled) White else Color(0xFFF0F1F3),
            shadowElevation = if (enabled) 4.dp else 0.dp,
            border = if (enabled) null else BorderStroke(1.dp, HairlineGray),
            modifier = Modifier
                .size(60.dp)
                .graphicsLayer {
                    scaleX = scale
                    scaleY = scale
                }
        ) {
            Box(
                contentAlignment = Alignment.Center,
                modifier = Modifier.size(60.dp)
            ) {
                Icon(
                    imageVector = icon,
                    contentDescription = label,
                    tint = if (enabled) iconTint else MutedGray,
                    modifier = Modifier.size(24.dp)
                )
            }
        }
        Spacer(modifier = Modifier.height(6.dp))
        Text(
            text = label,
            fontSize = 11.sp,
            fontWeight = FontWeight.Medium,
            fontFamily = Poppins,
            color = if (enabled) MutedGray else Color(0xFFC0C2C7)
        )
    }
}

data class CircularAction(
    val icon: ImageVector,
    val label: String,
    val enabled: Boolean = true,
    val onClick: () -> Unit
)

@Composable
fun CircularIconRow(
    actions: List<CircularAction>,
    modifier: Modifier = Modifier
) {
    Row(
        modifier = modifier
            .fillMaxWidth()
            .padding(horizontal = 40.dp),
        horizontalArrangement = Arrangement.SpaceEvenly
    ) {
        actions.forEach { action ->
            CircularIconButton(
                icon = action.icon,
                label = action.label,
                onClick = action.onClick,
                enabled = action.enabled
            )
        }
    }
}

@Composable
fun BottomSheetPanel(
    headline: String,
    headlineValue: String,
    headlineValueColor: Color,
    subtext: String,
    progress: Float,
    progressColor: Color,
    captionIcon: ImageVector,
    captionText: String,
    modifier: Modifier = Modifier,
    extraContent: (@Composable () -> Unit)? = null,
    headlineValueSuffix: String = "",
    statCards: @Composable () -> Unit
) {
    var sheetVisible by remember { mutableStateOf(false) }
    LaunchedEffect(Unit) {
        delay(200)
        sheetVisible = true
    }
    val offsetY by animateFloatAsState(
        targetValue = if (sheetVisible) 0f else 80f,
        animationSpec = tween(500, easing = FastOutSlowInEasing),
        label = "sheetSlide"
    )
    Surface(
        modifier = modifier
            .fillMaxWidth()
            .graphicsLayer { translationY = offsetY },
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
                    fontSize = 20.sp,
                    fontWeight = FontWeight.SemiBold,
                    fontFamily = Poppins,
                    color = NearBlack
                )
                Row(
                    verticalAlignment = Alignment.Bottom
                ) {
                    if (headlineValueSuffix.isNotEmpty()) {
                        Text(
                            text = headlineValue,
                            fontSize = 34.sp,
                            fontWeight = FontWeight.Bold,
                            fontFamily = Poppins,
                            color = headlineValueColor
                        )
                        Text(
                            text = headlineValueSuffix,
                            fontSize = 16.sp,
                            fontWeight = FontWeight.Medium,
                            fontFamily = Poppins,
                            color = headlineValueColor,
                            modifier = Modifier.padding(bottom = 4.dp, start = 2.dp)
                        )
                    } else {
                        Text(
                            text = headlineValue,
                            fontSize = 28.sp,
                            fontWeight = FontWeight.Bold,
                            fontFamily = Poppins,
                            color = headlineValueColor
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
                    tint = MutedGray,
                    modifier = Modifier.size(15.dp)
                )
                Spacer(modifier = Modifier.width(6.dp))
                Text(
                    text = captionText,
                    fontSize = 12.sp,
                    fontWeight = FontWeight.Normal,
                    fontFamily = Poppins,
                    color = MutedGray
                )
            }
            if (extraContent != null) {
                Spacer(modifier = Modifier.height(16.dp))
                extraContent()
            }
            Spacer(modifier = Modifier.height(18.dp))
            statCards()
            Spacer(modifier = Modifier.height(16.dp))
        }
    }
}

@Composable
fun StatCardPair(
    card1: StatCardData,
    card2: StatCardData
) {
    Row(
        modifier = Modifier.fillMaxWidth(),
        horizontalArrangement = Arrangement.spacedBy(12.dp)
    ) {
        StatCard(
            data = card1,
            modifier = Modifier.weight(1f)
        )
        StatCard(
            data = card2,
            modifier = Modifier.weight(1f)
        )
    }
}

data class StatCardData(
    val label: String,
    val value: String,
    val unit: String,
    val icon: ImageVector,
    val valueColor: Color = NearBlack
)

@Composable
fun StatCard(
    data: StatCardData,
    modifier: Modifier = Modifier
) {
    Card(
        modifier = modifier,
        shape = RoundedCornerShape(18.dp),
        colors = CardDefaults.cardColors(
            containerColor = Color(0xFFFAFBFC)
        ),
        border = BorderStroke(1.dp, HairlineGray),
        elevation = CardDefaults.cardElevation(defaultElevation = 0.dp)
    ) {
        Column(
            modifier = Modifier
                .fillMaxWidth()
                .padding(16.dp)
        ) {
            Row(
                modifier = Modifier.fillMaxWidth(),
                horizontalArrangement = Arrangement.SpaceBetween,
                verticalAlignment = Alignment.CenterVertically
            ) {
                Text(
                    text = data.label,
                    fontSize = 12.sp,
                    fontWeight = FontWeight.Medium,
                    fontFamily = Poppins,
                    color = MutedGray
                )
                Surface(
                    shape = RoundedCornerShape(8.dp),
                    color = Color(0xFFF0F1F3),
                    modifier = Modifier.size(28.dp)
                ) {
                    Box(
                        contentAlignment = Alignment.Center,
                        modifier = Modifier.size(28.dp)
                    ) {
                        Icon(
                            imageVector = data.icon,
                            contentDescription = data.label,
                            tint = DarkGray,
                            modifier = Modifier.size(15.dp)
                        )
                    }
                }
            }
            Spacer(modifier = Modifier.height(10.dp))
            Row(
                verticalAlignment = Alignment.Bottom
            ) {
                Text(
                    text = data.value,
                    fontSize = 24.sp,
                    fontWeight = FontWeight.Bold,
                    fontFamily = Poppins,
                    color = data.valueColor
                )
                if (data.unit.isNotEmpty()) {
                    Spacer(modifier = Modifier.width(3.dp))
                    Text(
                        text = data.unit,
                        fontSize = 13.sp,
                        fontWeight = FontWeight.Normal,
                        fontFamily = Poppins,
                        color = MutedGray,
                        modifier = Modifier.padding(bottom = 3.dp)
                    )
                }
            }
        }
    }
}

@Composable
fun SliderRow(
    minValue: Float,
    maxValue: Float,
    currentValue: Float,
    onValueChange: (Float) -> Unit,
    modifier: Modifier = Modifier,
    trackGradient: List<Color> = listOf(MintGreen, AlertRed)
) {
    val range = maxValue - minValue
    val normalizedValue = (currentValue - minValue) / range
    Row(
        modifier = modifier.fillMaxWidth(),
        verticalAlignment = Alignment.CenterVertically
    ) {
        Surface(
            shape = CircleShape,
            color = White,
            shadowElevation = 3.dp,
            border = BorderStroke(1.dp, HairlineGray),
            modifier = Modifier.size(36.dp)
        ) {
            Box(
                contentAlignment = Alignment.Center,
                modifier = Modifier.size(36.dp)
            ) {
                Text(
                    text = "−",
                    fontSize = 20.sp,
                    fontWeight = FontWeight.Medium,
                    fontFamily = Poppins,
                    color = DarkGray
                )
            }
        }
        Spacer(modifier = Modifier.width(12.dp))
        Box(
            modifier = Modifier
                .weight(1f)
                .height(36.dp),
            contentAlignment = Alignment.CenterStart
        ) {
            Box(
                modifier = Modifier
                    .fillMaxWidth()
                    .height(6.dp)
                    .clip(RoundedCornerShape(3.dp))
                    .background(Brush.horizontalGradient(trackGradient))
            )
            Surface(
                shape = CircleShape,
                color = White,
                shadowElevation = 6.dp,
                border = BorderStroke(2.dp, MintGreen),
                modifier = Modifier
                    .size(24.dp)
                    .graphicsLayer { translationX = (this.size.width * normalizedValue).px }
            ) {}
        }
        Spacer(modifier = Modifier.width(12.dp))
        Surface(
            shape = CircleShape,
            color = White,
            shadowElevation = 3.dp,
            border = BorderStroke(1.dp, HairlineGray),
            modifier = Modifier.size(36.dp)
        ) {
            Box(
                contentAlignment = Alignment.Center,
                modifier = Modifier.size(36.dp)
            ) {
                Text(
                    text = "+",
                    fontSize = 20.sp,
                    fontWeight = FontWeight.Medium,
                    fontFamily = Poppins,
                    color = DarkGray
                )
            }
        }
    }
}

private val Float.px: Float get() = this

enum class SettingsTrailingType {
    CHEVRON,
    SWITCH,
    VALUE
}

@Composable
fun SettingsRow(
    icon: ImageVector,
    title: String,
    modifier: Modifier = Modifier,
    subtitle: String? = null,
    trailingType: SettingsTrailingType = SettingsTrailingType.CHEVRON,
    trailingValue: String? = null,
    switchChecked: Boolean = false,
    onSwitchChange: ((Boolean) -> Unit)? = null,
    statusDotColor: Color? = null,
    showCopyIcon: Boolean = false
) {
    Row(
        modifier = modifier
            .fillMaxWidth()
            .padding(horizontal = 16.dp, vertical = 14.dp),
        verticalAlignment = Alignment.CenterVertically
    ) {
        Surface(
            shape = RoundedCornerShape(10.dp),
            color = Color(0xFFF0F1F3),
            modifier = Modifier.size(36.dp)
        ) {
            Box(
                contentAlignment = Alignment.Center,
                modifier = Modifier.size(36.dp)
            ) {
                Icon(
                    imageVector = icon,
                    contentDescription = title,
                    tint = DarkGray,
                    modifier = Modifier.size(18.dp)
                )
            }
        }
        Spacer(modifier = Modifier.width(14.dp))
        Column(
            modifier = Modifier.weight(1f)
        ) {
            Text(
                text = title,
                fontSize = 15.sp,
                fontWeight = FontWeight.Medium,
                fontFamily = Poppins,
                color = NearBlack
            )
            if (subtitle != null) {
                Text(
                    text = subtitle,
                    fontSize = 12.sp,
                    fontWeight = FontWeight.Normal,
                    fontFamily = Poppins,
                    color = MutedGray
                )
            }
        }
        when (trailingType) {
            SettingsTrailingType.CHEVRON -> {
                Icon(
                    imageVector = Icons.AutoMirrored.Filled.KeyboardArrowRight,
                    contentDescription = "Open",
                    tint = MutedGray,
                    modifier = Modifier.size(22.dp)
                )
            }
            SettingsTrailingType.SWITCH -> {
                Switch(
                    checked = switchChecked,
                    onCheckedChange = onSwitchChange,
                    colors = SwitchDefaults.colors(
                        checkedThumbColor = White,
                        checkedTrackColor = MintGreen,
                        uncheckedThumbColor = White,
                        uncheckedTrackColor = HairlineGray
                    )
                )
            }
            SettingsTrailingType.VALUE -> {
                Row(
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    if (statusDotColor != null) {
                        Box(
                            modifier = Modifier
                                .size(8.dp)
                                .clip(CircleShape)
                                .background(statusDotColor)
                        )
                        Spacer(modifier = Modifier.width(6.dp))
                    }
                    Text(
                        text = trailingValue ?: "",
                        fontSize = 14.sp,
                        fontWeight = FontWeight.Medium,
                        fontFamily = if (showCopyIcon) FontFamily.Monospace else Poppins,
                        color = if (statusDotColor == AlertRed) AlertRed else DarkGray
                    )
                    if (showCopyIcon) {
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
        }
    }
}

@Composable
fun SectionHeader(
    title: String,
    modifier: Modifier = Modifier
) {
    Text(
        text = title,
        fontSize = 13.sp,
        fontWeight = FontWeight.SemiBold,
        fontFamily = Poppins,
        color = MutedGray,
        modifier = modifier.padding(start = 16.dp, top = 20.dp, bottom = 8.dp),
        letterSpacing = 1.sp
    )
}

@Composable
fun SettingsCard(
    modifier: Modifier = Modifier,
    content: @Composable () -> Unit
) {
    Card(
        modifier = modifier
            .fillMaxWidth()
            .padding(horizontal = 20.dp),
        shape = RoundedCornerShape(20.dp),
        colors = CardDefaults.cardColors(containerColor = White),
        border = BorderStroke(1.dp, HairlineGray),
        elevation = CardDefaults.cardElevation(defaultElevation = 2.dp)
    ) {
        Column {
            content()
        }
    }
}
