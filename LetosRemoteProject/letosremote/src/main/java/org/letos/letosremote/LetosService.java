package org.letos.letosremote;

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.content.pm.ServiceInfo;
import android.net.wifi.WifiManager;
import android.os.Build;
import android.os.IBinder;
import android.os.PowerManager;
import android.util.Log;

import java.util.ArrayList;
import java.util.List;

import androidx.core.app.NotificationCompat;
import androidx.core.app.ServiceCompat;
import androidx.core.content.ContextCompat;

import org.letos.letosremote.internal.LetosListener;
import org.letos.letosremote.internal.Utils;

/**
 * Created by Pawel Salawa on 10.02.15.
 */
public class LetosService extends Service {
    private static final String ACTION_START = "org.letos.letosremote.action.START";
    private static final String EXTRA_PORT = "org.letos.letosremote.extra.PORT";
    private static final String EXTRA_IP_ADDRESS = "org.letos.letosremote.extra.IP_ADDRESS";
    private static final String EXTRA_PASSWORD = "org.letos.letosremote.extra.PASSWORD";
    private static final String EXTRA_IP_BLACKLIST = "org.letos.letosremote.extra.IP_BLACKLIST";
    private static final String EXTRA_IP_WHITELIST = "org.letos.letosremote.extra.IP_WHITELIST";
    private static final String NOTIFICATION_CHANNEL_ID = "org.letos.letosremote.listener";
    private static final int NOTIFICATION_ID = 12121;
    private static final String PARTIAL_WAKE_LOCK_TAG = "org.letos.letosremote:listener_cpu";
    private static final String WIFI_WAKE_LOCK_TAG = "org.letos.letosremote:listener_wifi";

    public static final int DEFAULT_PORT = 12121;
    private static LetosService staticInstance;
    private static volatile boolean running = false;
    private static Context applicationContext;

    private LetosListener listener;
    private Thread listenerThread;
    private PowerManager.WakeLock cpuWakeLock;
    private WifiManager.WifiLock wifiWakeLock;

    private int port = DEFAULT_PORT;
    private String ipAddress;
    private String password;
    private List<String> ipBlackList = new ArrayList<>();
    private List<String> ipWhiteList = new ArrayList<>();

    public void start(Context context) {
        if (running) {
            return;
        }

        applicationContext = context.getApplicationContext();
        Intent intent = new Intent(applicationContext, LetosService.class);
        intent.setAction(ACTION_START);
        intent.putExtra(EXTRA_PORT, port);
        intent.putExtra(EXTRA_IP_ADDRESS, ipAddress);
        intent.putExtra(EXTRA_PASSWORD, password);
        intent.putStringArrayListExtra(EXTRA_IP_BLACKLIST, new ArrayList<>(ipBlackList));
        intent.putStringArrayListExtra(EXTRA_IP_WHITELIST, new ArrayList<>(ipWhiteList));
        ContextCompat.startForegroundService(applicationContext, intent);
    }

    public void stop() {
        if (!running) {
            return;
        }

        if (applicationContext == null) {
            Log.w(Utils.LOG_TAG, "Cannot stop LetosService because it has no application context.");
            return;
        }

        applicationContext.stopService(new Intent(applicationContext, LetosService.class));
    }

    public static LetosService instance() {
        if (staticInstance == null) {
            staticInstance = new LetosService();
        }
        return staticInstance;
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        applicationContext = getApplicationContext();

        if (intent != null && ACTION_START.equals(intent.getAction())) {
            applyConfiguration(intent);
        }

        createNotificationChannel();
        ServiceCompat.startForeground(
                this,
                NOTIFICATION_ID,
                buildNotification(),
                ServiceInfo.FOREGROUND_SERVICE_TYPE_DATA_SYNC
        );

        if (!running) {
            startListener();
        }

        return START_REDELIVER_INTENT;
    }

    @Override
    public void onDestroy() {
        shutdownListener();
        stopForegroundCompat();
        super.onDestroy();
    }

    public void setPassword(String password) {
        this.password = password;
    }

    public void addIpToBlackList(String ip) {
        ipBlackList.add(ip);
    }

    public void addIpToWhiteList(String ip) {
        ipWhiteList.add(ip);
    }

    public void setIpBlackList(String... ip) {
        ipBlackList.clear();
        for (String singleIp : ip) {
            ipBlackList.add(singleIp);
        }
    }

    public void setIpWhiteList(String... ip) {
        ipWhiteList.clear();
        for (String singleIp : ip) {
            ipWhiteList.add(singleIp);
        }
    }

    public void setPort(int port) {
        this.port = port;
    }

    public boolean isRunning() {
        return running;
    }

    public void setIpAddress(String ipAddress) {
        this.ipAddress = ipAddress;
    }

    private void applyConfiguration(Intent intent) {
        port = intent.getIntExtra(EXTRA_PORT, DEFAULT_PORT);
        ipAddress = intent.getStringExtra(EXTRA_IP_ADDRESS);
        password = intent.getStringExtra(EXTRA_PASSWORD);

        ArrayList<String> blackList = intent.getStringArrayListExtra(EXTRA_IP_BLACKLIST);
        ipBlackList = blackList != null ? new ArrayList<>(blackList) : new ArrayList<>();

        ArrayList<String> whiteList = intent.getStringArrayListExtra(EXTRA_IP_WHITELIST);
        ipWhiteList = whiteList != null ? new ArrayList<>(whiteList) : new ArrayList<>();
    }

    private void startListener() {
        acquireWakeLocks();

        listener = new LetosListener(getApplicationContext());
        listener.setPort(port);
        listener.setIpAddress(ipAddress);
        listener.setPassword(password);
        listener.setIpBlackList(new ArrayList<>(ipBlackList));
        listener.setIpWhiteList(new ArrayList<>(ipWhiteList));

        listenerThread = new Thread(listener, "LetosListener");
        listenerThread.start();
        running = true;
        Log.d(Utils.LOG_TAG, "Started foreground LetosService on port " + port);
    }

    private void shutdownListener() {
        Log.d(Utils.LOG_TAG, "Shutting down LetosService instance.");

        if (listener != null) {
            listener.close();
        }

        if (listenerThread != null) {
            try {
                listenerThread.join();
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
            }
        }

        listener = null;
        listenerThread = null;
        running = false;
        releaseWakeLocks();
    }

    private void acquireWakeLocks() {
        PowerManager powerManager = (PowerManager) getApplicationContext().getSystemService(Context.POWER_SERVICE);
        if (powerManager != null && cpuWakeLock == null) {
            cpuWakeLock = powerManager.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK, PARTIAL_WAKE_LOCK_TAG);
            cpuWakeLock.setReferenceCounted(false);
        }
        if (cpuWakeLock != null && !cpuWakeLock.isHeld()) {
            cpuWakeLock.acquire();
        }

        WifiManager wifiManager = (WifiManager) getApplicationContext().getSystemService(Context.WIFI_SERVICE);
        if (wifiManager == null) {
            Log.w(Utils.LOG_TAG, "WifiManager unavailable, continuing without WifiLock.");
            return;
        }
        if (wifiWakeLock == null) {
            wifiWakeLock = wifiManager.createWifiLock(WifiManager.WIFI_MODE_FULL_HIGH_PERF, WIFI_WAKE_LOCK_TAG);
            wifiWakeLock.setReferenceCounted(false);
        }
        if (!wifiWakeLock.isHeld()) {
            wifiWakeLock.acquire();
        }
    }

    private void releaseWakeLocks() {
        if (wifiWakeLock != null && wifiWakeLock.isHeld()) {
            wifiWakeLock.release();
        }
        if (cpuWakeLock != null && cpuWakeLock.isHeld()) {
            cpuWakeLock.release();
        }
    }

    private void createNotificationChannel() {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.O) {
            return;
        }

        NotificationManager notificationManager = getSystemService(NotificationManager.class);
        if (notificationManager == null) {
            Log.w(Utils.LOG_TAG, "NotificationManager unavailable, foreground notification channel not created.");
            return;
        }

        NotificationChannel channel = new NotificationChannel(
                NOTIFICATION_CHANNEL_ID,
                getString(R.string.letos_service_notification_channel_name),
                NotificationManager.IMPORTANCE_LOW
        );
        channel.setDescription(getString(R.string.letos_service_notification_channel_description));
        notificationManager.createNotificationChannel(channel);
    }

    private Notification buildNotification() {
        return new NotificationCompat.Builder(this, NOTIFICATION_CHANNEL_ID)
                .setSmallIcon(android.R.drawable.stat_sys_upload_done)
                .setContentTitle(getString(R.string.letos_service_notification_title))
                .setContentText(getString(R.string.letos_service_notification_text, port))
                .setOngoing(true)
                .setSilent(true)
                .setCategory(NotificationCompat.CATEGORY_SERVICE)
                .build();
    }

    private void stopForegroundCompat() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
            stopForeground(STOP_FOREGROUND_REMOVE);
        } else {
            stopForeground(true);
        }
    }
}
