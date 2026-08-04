package com.rork.safecellai.ui.screens

import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.interaction.MutableInteractionSource
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.PaddingValues
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.RowScope
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
import androidx.compose.material.icons.filled.LocalFireDepartment
import androidx.compose.material.icons.filled.LocalHospital
import androidx.compose.material.icons.filled.LocationOn
import androidx.compose.material.icons.filled.MyLocation
import androidx.compose.material.icons.filled.Notifications
import androidx.compose.material.icons.filled.Route
import androidx.compose.material.icons.filled.Speed
import androidx.compose.material3.Icon
import androidx.compose.material3.IconButton
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
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.layout.ContentScale
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import coil3.compose.AsyncImage
import coil3.request.ImageRequest
import coil3.request.crossfade
import com.rork.safecellai.SafeCellAssets
import com.rork.safecellai.ui.theme.HairlineGray
import com.rork.safecellai.ui.theme.MintGreen
import com.rork.safecellai.ui.theme.MutedGray
import com.rork.safecellai.ui.theme.NearBlack
import com.rork.safecellai.ui.theme.Poppins
import com.rork.safecellai.ui.theme.White
import com.rork.safecellai.ui.widgets.AnimatedProgressBar
import com.rork.safecellai.ui.widgets.StatCardData
import com.rork.safecellai.ui.widgets.StatCardPair

@Composable
fun MapsScreen(
    contentPadding: PaddingValues = PaddingValues(0.dp)
) {
    val scrollState = rememberScrollState()
    val context = LocalContext.current
    var showFireStation by remember { mutableStateOf(true) }

    Column(
        modifier = Modifier
            .fillMaxSize()
            .verticalScroll(scrollState)
            .padding(bottom = contentPadding.calculateBottomPadding())
    ) {
        Box(
            modifier = Modifier
                .fillMaxWidth()
                .height(360.dp)
        ) {
            AsyncImage(
                model = ImageRequest.Builder(context)
                    .data(SafeCellAssets.MAP_PLACEHOLDER)
                    .crossfade(true)
                    .build(),
                contentDescription = "Map View",
                contentScale = ContentScale.Crop,
                modifier = Modifier.fillMaxSize()
            )

            Surface(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(top = 16.dp)
                    .padding(horizontal = 20.dp),
                shape = RoundedCornerShape(20.dp),
                color = White.copy(alpha = 0.9f),
                tonalElevation = 0.dp,
                shadowElevation = 8.dp
            ) {
                Row(
                    modifier = Modifier
                        .fillMaxWidth()
                        .padding(horizontal = 20.dp, vertical = 12.dp),
                    horizontalArrangement = Arrangement.SpaceBetween,
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Text(
                        text = "Location",
                        fontSize = 20.sp,
                        fontWeight = FontWeight.SemiBold,
                        fontFamily = Poppins,
                        color = NearBlack
                    )
                    IconButton(onClick = {}) {
                        Icon(
                            imageVector = Icons.Filled.Notifications,
                            contentDescription = "Notifications",
                            tint = NearBlack,
                            modifier = Modifier.size(24.dp)
                        )
                    }
                }
            }
        }

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
                        .clip(RoundedCornerShape(12.dp))
                        .background(Color(0xFFF5F6F8))
                        .padding(4.dp),
                    horizontalArrangement = Arrangement.SpaceEvenly
                ) {
                    ToggleChip(
                        text = "Fire Station",
                        selected = showFireStation,
                        onClick = { showFireStation = true }
                    )
                    ToggleChip(
                        text = "Hospital",
                        selected = !showFireStation,
                        onClick = { showFireStation = false }
                    )
                }

                Spacer(modifier = Modifier.height(20.dp))

                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween,
                    verticalAlignment = Alignment.Bottom
                ) {
                    Text(
                        text = if (showFireStation) "Nearest Fire Station" else "Nearest Hospital",
                        fontSize = 20.sp,
                        fontWeight = FontWeight.SemiBold,
                        fontFamily = Poppins,
                        color = NearBlack
                    )
                    Icon(
                        imageVector = if (showFireStation) Icons.Filled.LocalFireDepartment else Icons.Filled.LocalHospital,
                        contentDescription = null,
                        tint = MintGreen,
                        modifier = Modifier.size(20.dp)
                    )
                }
                Spacer(modifier = Modifier.height(4.dp))
                Text(
                    text = if (showFireStation) "Station 42 — Downtown" else "City General Hospital",
                    fontSize = 13.sp,
                    fontWeight = FontWeight.Normal,
                    fontFamily = Poppins,
                    color = MutedGray
                )
                Spacer(modifier = Modifier.height(14.dp))

                AnimatedProgressBar(
                    progress = 0.35f,
                    barColor = MintGreen
                )
                Spacer(modifier = Modifier.height(10.dp))

                Row(
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Icon(
                        imageVector = Icons.Filled.Route,
                        contentDescription = null,
                        tint = MutedGray,
                        modifier = Modifier.size(15.dp)
                    )
                    Spacer(modifier = Modifier.width(6.dp))
                    Text(
                        text = "2.4 km away — 6 min drive",
                        fontSize = 12.sp,
                        fontWeight = FontWeight.Normal,
                        fontFamily = Poppins,
                        color = MutedGray
                    )
                }

                Spacer(modifier = Modifier.height(18.dp))

                StatCardPair(
                    card1 = StatCardData(
                        label = "Current Speed",
                        value = "0",
                        unit = " km/h",
                        icon = Icons.Filled.Speed
                    ),
                    card2 = StatCardData(
                        label = "Coordinates",
                        value = "37.77°N",
                        unit = "",
                        icon = Icons.Filled.LocationOn
                    )
                )
                Spacer(modifier = Modifier.height(16.dp))
            }
        }
    }
}

@Composable
private fun RowScope.ToggleChip(
    text: String,
    selected: Boolean,
    onClick: () -> Unit
) {
    Surface(
        modifier = Modifier
            .weight(1f)
            .clip(RoundedCornerShape(10.dp))
            .clickable(
                interactionSource = remember { MutableInteractionSource() },
                indication = null
            ) { onClick() },
        shape = RoundedCornerShape(10.dp),
        color = if (selected) White else Color.Transparent
    ) {
        Box(
            modifier = Modifier
                .fillMaxWidth()
                .padding(vertical = 8.dp),
            contentAlignment = Alignment.Center
        ) {
            Text(
                text = text,
                fontSize = 13.sp,
                fontWeight = if (selected) FontWeight.SemiBold else FontWeight.Medium,
                fontFamily = Poppins,
                color = if (selected) NearBlack else MutedGray
            )
        }
    }
}