#pragma once

static int busyScale = 0;

static void markBusy() {
  if (busyScale == 0) {
    digitalWrite(PIN_BUSY, 1);
  }

  busyScale++;
}

static void markIdle() {
  busyScale--;

  if (busyScale == 0) {
    digitalWrite(PIN_BUSY, 0);
  }
}
