# Troubleshooting Guide

This guide helps resolve common issues encountered while using the **Guitar Diagnostic Analyzer**.

## Audio Issues

### "No Audio Devices Found"
*   **Cause**: The application cannot detect any connected audio interfaces.
*   **Solution**:
    1. Check if your USB audio interface is properly connected and powered on.
    2. Try a different USB port.
    3. Restart the application.
    4. Reinstall your audio interface drivers.

### "Microphone Permission Denied"
*   **Cause**: Operating system privacy settings are blocking access to audio inputs.
*   **Solution**:
    *   **Windows**: Go to **Settings > Privacy > Microphone**. Ensure "Allow desktop apps to access your microphone" is turned **ON**.
    *   **macOS**: Go to **System Preferences > Security & Privacy > Privacy > Microphone** and check the box next to GuitarDiagnostics.

### Audio Clicks, Pops, or Crackling
*   **Cause**: The computer cannot process audio fast enough (buffer underrun) or the sample rate is mismatched.
*   **Solution**:
    1. Close other background applications.
    2. Increase the **Buffer Size** in your audio driver settings (if available).
    3. Ensure your interface is set to a standard sample rate (e.g., 44.1 kHz or 48 kHz).

### High Background Noise
*   **Cause**: Electrical interference or low input gain.
*   **Solution**:
    1. Move away from computer monitors and fluorescent lights (single-coil pickups detect 60Hz hum).
    2. Use a shielded guitar cable.
    3. Adjust the noise gate threshold in the Audio Monitor (if available).

---

## Analysis Issues

### Readings Fluctuate Wildly (Unstable)
*   **Cause**: The input signal is too weak or too strong (clipping).
*   **Solution**: Adjust your interface gain so the input meter in the Audio Monitor hits the "green" or "yellow" zone, but never "red".

### Fret Buzz Detected Everywhere
*   **Cause**: High background noise or "string rattle" from aggressive picking.
*   **Solution**:
    1. Pluck the string with moderate, consistent force.
    2. Dampen the strings you aren't playing to prevent sympathetic resonance.

### Intonation Tuner Won't Lock On
*   **Cause**: Old strings or poor signal.
*   **Solution**:
    1. Switch to the bridge pickup for a clearer fundamental frequency.
    2. Tone knob at 10.
    3. Replace strings if they are old/rusty.

---

## Performance Issues

### High CPU Usage
*   **Cause**: Running on older hardware or Debug build.
*   **Solution**:
    1. Ensure you are running the **Release** build of the application.
    2. Close browser tabs and heavy background processes.

---

## Seeking Further Help

If your issue isn't listed here, please check the [GitHub Issues](https://github.com/Konstantysz/guitar-diagnostics/issues) page or contact support.
