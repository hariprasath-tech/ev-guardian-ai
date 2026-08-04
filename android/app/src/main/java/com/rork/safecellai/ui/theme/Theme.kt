package com.rork.safecellai.ui.theme

import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.lightColorScheme
import androidx.compose.runtime.Composable

val SafeCellColors = lightColorScheme(
    primary = MintGreen,
    onPrimary = White,
    secondary = LightMint,
    onSecondary = NearBlack,
    tertiary = AlertRed,
    onTertiary = White,
    background = SoftGray,
    onBackground = NearBlack,
    surface = White,
    onSurface = NearBlack,
    surfaceVariant = HairlineGray,
    onSurfaceVariant = MutedGray,
    error = AlertRed,
    onError = White,
    outline = HairlineGray,
    outlineVariant = HairlineGray
)

@Composable
fun AppTheme(
    content: @Composable () -> Unit
) {
    MaterialTheme(
        colorScheme = SafeCellColors,
        typography = SafeCellTypography,
        content = content
    )
}
