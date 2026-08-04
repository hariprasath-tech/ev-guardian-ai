package com.rork.safecellai.ui.screens

import androidx.compose.foundation.BorderStroke
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
import androidx.compose.material.icons.filled.Info
import androidx.compose.material.icons.filled.Language
import androidx.compose.material.icons.filled.Logout
import androidx.compose.material.icons.filled.Memory
import androidx.compose.material.icons.filled.Palette
import androidx.compose.material.icons.filled.Person
import androidx.compose.material.icons.filled.Settings
import androidx.compose.material.icons.filled.Thermostat
import androidx.compose.material.icons.filled.Warning
import androidx.compose.material.icons.filled.Wifi
import androidx.compose.material3.Card
import androidx.compose.material3.CardDefaults
import androidx.compose.material3.Icon
import androidx.compose.material3.Surface
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.Brush
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.rork.safecellai.ui.theme.AlertRed
import com.rork.safecellai.ui.theme.HairlineGray
import com.rork.safecellai.ui.theme.MintGreen
import com.rork.safecellai.ui.theme.MutedGray
import com.rork.safecellai.ui.theme.NearBlack
import com.rork.safecellai.ui.theme.Poppins
import com.rork.safecellai.ui.theme.SoftGray
import com.rork.safecellai.ui.theme.White
import com.rork.safecellai.ui.widgets.SectionHeader
import com.rork.safecellai.ui.widgets.SettingsCard
import com.rork.safecellai.ui.widgets.SettingsRow
import com.rork.safecellai.ui.widgets.SettingsTrailingType

@Composable
fun SettingsScreen(
    contentPadding: PaddingValues = PaddingValues(0.dp)
) {
    val scrollState = rememberScrollState()
    var fireAlertOn by remember { mutableStateOf(true) }
    var gasAlertOn by remember { mutableStateOf(true) }
    var batteryAlertOn by remember { mutableStateOf(false) }
    var tempCelsius by remember { mutableStateOf(true) }
    var gasPpm by remember { mutableStateOf(true) }

    Column(
        modifier = Modifier
            .fillMaxSize()
            .verticalScroll(scrollState)
            .padding(bottom = contentPadding.calculateBottomPadding())
    ) {
        ProfileHeader()

        SectionHeader(title = "NOTIFICATIONS")
        SettingsCard {
            SettingsRow(
                icon = Icons.Filled.Warning,
                title = "Fire Alert",
                trailingType = SettingsTrailingType.SWITCH,
                switchChecked = fireAlertOn,
                onSwitchChange = { fireAlertOn = it }
            )
            DividerLine()
            SettingsRow(
                icon = Icons.Filled.Warning,
                title = "Gas Alert",
                trailingType = SettingsTrailingType.SWITCH,
                switchChecked = gasAlertOn,
                onSwitchChange = { gasAlertOn = it }
            )
            DividerLine()
            SettingsRow(
                icon = Icons.Filled.Warning,
                title = "Battery Alert",
                trailingType = SettingsTrailingType.SWITCH,
                switchChecked = batteryAlertOn,
                onSwitchChange = { batteryAlertOn = it }
            )
        }

        SectionHeader(title = "UNITS")
        SettingsCard {
            SettingsRow(
                icon = Icons.Filled.Thermostat,
                title = "Temperature",
                subtitle = "Display unit preference",
                trailingType = SettingsTrailingType.SWITCH,
                switchChecked = tempCelsius,
                onSwitchChange = { tempCelsius = it }
            )
            DividerLine()
            SettingsRow(
                icon = Icons.Filled.Settings,
                title = "Gas Reading",
                subtitle = "Display unit preference",
                trailingType = SettingsTrailingType.SWITCH,
                switchChecked = gasPpm,
                onSwitchChange = { gasPpm = it }
            )
        }

        SectionHeader(title = "CONNECTED DEVICES")
        SettingsCard {
            SettingsRow(
                icon = Icons.Filled.Memory,
                title = "Device Name",
                trailingType = SettingsTrailingType.CHEVRON
            )
            DividerLine()
            SettingsRow(
                icon = Icons.Filled.Memory,
                title = "Device MAC Address",
                trailingType = SettingsTrailingType.VALUE,
                trailingValue = "A4:CF:12:8B:3E:9D",
                showCopyIcon = true
            )
            DividerLine()
            SettingsRow(
                icon = Icons.Filled.Wifi,
                title = "Connection Status",
                trailingType = SettingsTrailingType.VALUE,
                trailingValue = "Connected",
                statusDotColor = MintGreen
            )
        }

        SectionHeader(title = "APP")
        SettingsCard {
            SettingsRow(
                icon = Icons.Filled.Palette,
                title = "Theme",
                subtitle = "Light",
                trailingType = SettingsTrailingType.CHEVRON
            )
            DividerLine()
            SettingsRow(
                icon = Icons.Filled.Language,
                title = "Language",
                subtitle = "English (US)",
                trailingType = SettingsTrailingType.CHEVRON
            )
            DividerLine()
            SettingsRow(
                icon = Icons.Filled.Info,
                title = "About",
                subtitle = "SafeCell AI v1.0.0",
                trailingType = SettingsTrailingType.CHEVRON
            )
        }

        Spacer(modifier = Modifier.height(24.dp))

        Surface(
            modifier = Modifier
                .fillMaxWidth()
                .padding(horizontal = 20.dp)
                .clip(RoundedCornerShape(16.dp))
                .clickable(
                    interactionSource = remember { MutableInteractionSource() },
                    indication = null
                ) {},
            shape = RoundedCornerShape(16.dp),
            color = Color(0xFFFDF4F3),
            border = BorderStroke(1.dp, AlertRed.copy(alpha = 0.3f))
        ) {
            Row(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(vertical = 16.dp),
                horizontalArrangement = Arrangement.Center,
                verticalAlignment = Alignment.CenterVertically
            ) {
                Icon(
                    imageVector = Icons.Filled.Logout,
                    contentDescription = "Logout",
                    tint = AlertRed,
                    modifier = Modifier.size(20.dp)
                )
                Spacer(modifier = Modifier.width(8.dp))
                Text(
                    text = "Log Out",
                    fontSize = 15.sp,
                    fontWeight = FontWeight.SemiBold,
                    fontFamily = Poppins,
                    color = AlertRed
                )
            }
        }

        Spacer(modifier = Modifier.height(32.dp))
    }
}

@Composable
private fun ProfileHeader() {
    val gradientColors = listOf(Color(0xFFECEEF1), SoftGray)
    Box(
        modifier = Modifier
            .fillMaxWidth()
            .height(200.dp)
            .background(Brush.verticalGradient(gradientColors))
    ) {
        Column(
            modifier = Modifier
                .fillMaxWidth()
                .padding(top = 24.dp, start = 20.dp, end = 20.dp),
            horizontalAlignment = Alignment.CenterHorizontally
        ) {
            Surface(
                shape = CircleShape,
                color = MintGreen.copy(alpha = 0.15f),
                border = BorderStroke(2.dp, MintGreen.copy(alpha = 0.3f)),
                modifier = Modifier.size(80.dp)
            ) {
                Box(
                    contentAlignment = Alignment.Center,
                    modifier = Modifier.size(80.dp)
                ) {
                    Icon(
                        imageVector = Icons.Filled.Person,
                        contentDescription = "Profile",
                        tint = MintGreen,
                        modifier = Modifier.size(36.dp)
                    )
                }
            }
            Spacer(modifier = Modifier.height(12.dp))
            Text(
                text = "Alex Chen",
                fontSize = 20.sp,
                fontWeight = FontWeight.SemiBold,
                fontFamily = Poppins,
                color = NearBlack
            )
            Spacer(modifier = Modifier.height(6.dp))
            Surface(
                shape = RoundedCornerShape(50),
                color = White,
                border = BorderStroke(1.dp, HairlineGray),
                modifier = Modifier.clickable(
                    interactionSource = remember { MutableInteractionSource() },
                    indication = null
                ) {}
            ) {
                Row(
                    modifier = Modifier.padding(horizontal = 16.dp, vertical = 6.dp),
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Text(
                        text = "Edit Profile",
                        fontSize = 12.sp,
                        fontWeight = FontWeight.Medium,
                        fontFamily = Poppins,
                        color = MutedGray
                    )
                }
            }
        }
    }
}

@Composable
private fun DividerLine() {
    Box(
        modifier = Modifier
            .fillMaxWidth()
            .padding(horizontal = 16.dp)
            .height(1.dp)
            .background(HairlineGray)
    )
}
