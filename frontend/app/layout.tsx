import type { Metadata } from "next";
import "./globals.css";

export const metadata: Metadata = {
  title: "SafeCell AI — EV Battery Safety Dashboard",
  description: "Real-time EV battery monitoring and fire suppression dashboard with ESP32 hardware integration.",
};

export default function RootLayout({
  children,
}: Readonly<{
  children: React.ReactNode;
}>) {
  return (
    <html lang="en">
      <body suppressHydrationWarning>{children}</body>
    </html>
  );
}
