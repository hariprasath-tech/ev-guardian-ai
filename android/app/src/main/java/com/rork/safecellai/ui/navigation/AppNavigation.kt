package com.rork.safecellai.ui.navigation

import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Home
import androidx.compose.material.icons.filled.LocalFireDepartment
import androidx.compose.material.icons.filled.Map
import androidx.compose.material.icons.filled.Settings
import androidx.compose.material.icons.filled.Thermostat
import androidx.compose.material.icons.filled.Warning
import androidx.compose.material.icons.filled.Memory
import androidx.compose.material3.Icon
import androidx.compose.material3.NavigationBar
import androidx.compose.material3.NavigationBarItem
import androidx.compose.material3.NavigationBarItemDefaults
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.vector.ImageVector
import androidx.compose.foundation.layout.size
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import androidx.navigation.NavHostController
import androidx.navigation.compose.NavHost
import androidx.navigation.compose.composable
import androidx.navigation.compose.currentBackStackEntryAsState
import androidx.navigation.compose.rememberNavController
import com.rork.safecellai.ui.screens.EmergencyScreen
import com.rork.safecellai.ui.screens.Esp32ConnectionScreen
import com.rork.safecellai.ui.screens.FireSuppressionScreen
import com.rork.safecellai.ui.screens.GasMonitoringScreen
import com.rork.safecellai.ui.screens.HomeScreen
import com.rork.safecellai.ui.screens.MapsScreen
import com.rork.safecellai.ui.screens.SettingsScreen
import com.rork.safecellai.ui.theme.MintGreen
import com.rork.safecellai.ui.theme.MutedGray
import com.rork.safecellai.ui.theme.Poppins
import com.rork.safecellai.ui.theme.White

private data class BottomNavItem(
    val route: String,
    val label: String,
    val icon: ImageVector
)

private val bottomNavItems = listOf(
    BottomNavItem("home", "Home", Icons.Filled.Home),
    BottomNavItem("gas", "Gas", Icons.Filled.Thermostat),
    BottomNavItem("emergency", "SOS", Icons.Filled.Warning),
    BottomNavItem("suppression", "Fire", Icons.Filled.LocalFireDepartment),
    BottomNavItem("esp32", "Device", Icons.Filled.Memory),
    BottomNavItem("maps", "Maps", Icons.Filled.Map),
    BottomNavItem("settings", "Settings", Icons.Filled.Settings)
)

@Composable
fun AppNavigation() {
    val navController = rememberNavController()
    val navBackStackEntry by navController.currentBackStackEntryAsState()
    val currentRoute = navBackStackEntry?.destination?.route

    Scaffold(
        bottomBar = {
            if (currentRoute != null) {
                SafeCellBottomBar(
                    currentRoute = currentRoute,
                    onNavigate = { route ->
                        if (route != currentRoute) {
                            navController.navigate(route) {
                                popUpTo(navController.graph.startDestinationId) {
                                    saveState = true
                                }
                                launchSingleTop = true
                                restoreState = true
                            }
                        }
                    }
                )
            }
        },
        containerColor = White
    ) { innerPadding ->
        NavHost(
            navController = navController,
            startDestination = "home"
        ) {
            composable("home") { HomeScreen(contentPadding = innerPadding) }
            composable("gas") { GasMonitoringScreen(contentPadding = innerPadding) }
            composable("emergency") { EmergencyScreen(contentPadding = innerPadding) }
            composable("suppression") { FireSuppressionScreen(contentPadding = innerPadding) }
            composable("esp32") { Esp32ConnectionScreen(contentPadding = innerPadding) }
            composable("maps") { MapsScreen(contentPadding = innerPadding) }
            composable("settings") { SettingsScreen(contentPadding = innerPadding) }
        }
    }
}

@Composable
private fun SafeCellBottomBar(
    currentRoute: String,
    onNavigate: (String) -> Unit
) {
    NavigationBar(
        containerColor = White,
        tonalElevation = 0.dp,
        modifier = Modifier
    ) {
        bottomNavItems.forEach { item ->
            val selected = currentRoute == item.route
            NavigationBarItem(
                selected = selected,
                onClick = { onNavigate(item.route) },
                icon = {
                    Icon(
                        imageVector = item.icon,
                        contentDescription = item.label,
                        modifier = Modifier.size(24.dp)
                    )
                },
                label = {
                    Text(
                        text = item.label,
                        fontSize = 10.sp,
                        fontWeight = FontWeight.Medium,
                        fontFamily = Poppins
                    )
                },
                colors = NavigationBarItemDefaults.colors(
                    selectedIconColor = MintGreen,
                    selectedTextColor = MintGreen,
                    unselectedIconColor = MutedGray,
                    unselectedTextColor = MutedGray,
                    indicatorColor = MintGreen.copy(alpha = 0.12f)
                )
            )
        }
    }
}
