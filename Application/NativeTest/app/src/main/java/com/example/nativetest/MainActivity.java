/*
 * Copyright 2018 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
// Modified by Delphine Somerhausen (delsomer@gmail.com)

package com.example.nativetest;

import android.Manifest;
import android.app.Activity;
import android.content.pm.PackageManager;
import android.media.AudioManager;
import android.os.Bundle;
import android.text.Editable;
import android.text.TextWatcher;
import android.widget.*;
import androidx.annotation.NonNull;
import androidx.core.app.ActivityCompat;
import android.util.Log;
import android.view.View;
import com.google.android.material.textfield.TextInputEditText;

import java.util.Timer;
import java.util.TimerTask;


public class MainActivity extends Activity
		implements ActivityCompat.OnRequestPermissionsResultCallback {

	private static final String TAG = MainActivity.class.getName();
	private static final int AUDIO_EFFECT_REQUEST = 0;
	private static final int OBOE_API_AAUDIO = 0;
	private static final int OBOE_API_OPENSL_ES = 1;
	private static final double TAU_1 = 0.0001;
	private static final double TAU_2 = 0.001;
	private static final double TAU_3 = 0.01;
	private static final double TAU_4 = 0.5;

	private TextView statusText;
	private Button toggleEffectButton;
	private boolean isPlaying = false;
	private TextInputEditText tauInput;

	private int apiSelection = OBOE_API_AAUDIO;
	private double tau = TAU_2;
	private boolean mAAudioRecommended = true;
	private boolean nr = true;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);

		statusText = findViewById(R.id.status_view_text);
		toggleEffectButton = findViewById(R.id.button_toggle_effect);
		toggleEffectButton.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View view) {
				toggleEffect();
			}
		});
		toggleEffectButton.setText(getString(R.string.start_effect));

		((RadioGroup)findViewById(R.id.apiSelectionGroup)).check(R.id.aaudioButton);
		findViewById(R.id.aaudioButton).setOnClickListener(new RadioButton.OnClickListener(){
			@Override
			public void onClick(View v) {
				if (((RadioButton)v).isChecked()) {
					apiSelection = OBOE_API_AAUDIO;
				}
			}
		});
		findViewById(R.id.slesButton).setOnClickListener(new RadioButton.OnClickListener(){
			@Override
			public void onClick(View v) {
				if (((RadioButton)v).isChecked()) {
					apiSelection = OBOE_API_OPENSL_ES;
				}
			}
		});

		tauInput = findViewById(R.id.input);
		final RadioGroup tauSelection = findViewById(R.id.tauSelectionGroup);
		findViewById(R.id.TauButton1).setOnClickListener(new RadioButton.OnClickListener(){
			@Override
			public void onClick(View v) {
				if (((RadioButton)v).isChecked()) {
					tau = TAU_1;
					tauInput.setText(getString(R.string.tau1));
				}
			}
		});
		findViewById(R.id.TauButton2).setOnClickListener(new RadioButton.OnClickListener(){
			@Override
			public void onClick(View v) {
				if (((RadioButton)v).isChecked()) {
					tau = TAU_2;
					tauInput.setText(getString(R.string.tau2));
				}
			}
		});
		findViewById(R.id.TauButton3).setOnClickListener(new RadioButton.OnClickListener(){
			@Override
			public void onClick(View v) {
				if (((RadioButton)v).isChecked()) {
					tau = TAU_3;
					tauInput.setText(getString(R.string.tau3));
				}
			}
		});
		findViewById(R.id.TauButton4).setOnClickListener(new RadioButton.OnClickListener(){
			@Override
			public void onClick(View v) {
				if (((RadioButton)v).isChecked()) {
					tau = TAU_4;
					tauInput.setText(getString(R.string.tau4));
				}
			}
		});

		// Source : https://futurestud.io/tutorials/android-how-to-delay-changedtextevent-on-androids-edittext
		tauInput.addTextChangedListener(new TextWatcher() {
			Timer timer;

			@Override
			public void beforeTextChanged(CharSequence s, int start, int count, int after) {	}

			@Override
			public void onTextChanged(CharSequence s, int start, int before, int count) {
				if (timer != null) {
					timer.cancel();
				}
			}

			@Override
			public void afterTextChanged(Editable s) {
				timer = new Timer();
				timer.schedule(new TimerTask() {
					@Override
					public void run() {
						String text = tauInput.getText().toString();
						if(!text.isEmpty()) {
							tau = Double.parseDouble(text);
							if (tau < TAU_1){tau = TAU_1;tauInput.setText(getString(R.string.tau1));	}
							else if (tau > TAU_4){tau = TAU_4;tauInput.setText(getString(R.string.tau4));	}
							if(tau == 0.0001){tauSelection.check(R.id.TauButton1);}
							else if(tau == 0.001){tauSelection.check(R.id.TauButton2);}
							else if(tau == 0.01){tauSelection.check(R.id.TauButton3);}
							else if(tau == 0.5){tauSelection.check(R.id.TauButton4);}
							else{tauSelection.clearCheck();}
						}
						else{
							tauInput.setText(getString(R.string.tau2));
							tau = TAU_2;
						}
					}
				}, 1000); // 1000ms delay before the timer executes the „run“ method from TimerTask
			}
		});

		ToggleButton nrSwitch = findViewById(R.id.nrSwitch);
		nrSwitch.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener(){
			public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
				if (isChecked) {
					nr = true;
				} else {
					nr = false;
				}
			}
		});

		LiveEffectEngine.setDefaultStreamValues(this);
	}

	private void EnableUI(boolean enable) {
		if(apiSelection == OBOE_API_AAUDIO && !mAAudioRecommended)
		{
			apiSelection = OBOE_API_OPENSL_ES;
		}
		if(!mAAudioRecommended) {
			findViewById(R.id.aaudioButton).setEnabled(false);
		} else {
			findViewById(R.id.aaudioButton).setEnabled(enable);
		}

		findViewById(R.id.slesButton).setEnabled(enable);
		findViewById(R.id.TauButton1).setEnabled(enable);
		findViewById(R.id.TauButton2).setEnabled(enable);
		findViewById(R.id.TauButton3).setEnabled(enable);
		findViewById(R.id.TauButton4).setEnabled(enable);
		findViewById(R.id.input).setEnabled(enable);
		findViewById(R.id.nrSwitch).setEnabled(enable);

		((RadioGroup)findViewById(R.id.apiSelectionGroup))
				.check(apiSelection == OBOE_API_AAUDIO ? R.id.aaudioButton : R.id.slesButton);
	}

	@Override
	protected void onStart() {
		super.onStart();
		setVolumeControlStream(AudioManager.STREAM_MUSIC);
	}

	@Override
	protected void onResume() {
		super.onResume();
		LiveEffectEngine.create();
		mAAudioRecommended = LiveEffectEngine.isAAudioRecommended();
		EnableUI(true);
		LiveEffectEngine.setAPI(apiSelection);
		LiveEffectEngine.setNR(nr);
		LiveEffectEngine.setTau(tau);
	}
	@Override
	protected void onPause() {
		stopEffect();
		LiveEffectEngine.delete();
		super.onPause();
	}

	public void toggleEffect() {
		if (isPlaying) {
			stopEffect();
		} else {
			LiveEffectEngine.setAPI(apiSelection);
			LiveEffectEngine.setTau(tau);
			LiveEffectEngine.setNR(nr);
			startEffect();
		}
	}

	private void startEffect() {
		Log.d(TAG, "Attempting to start");

		if (!isRecordPermissionGranted()){
			requestRecordPermission();
			return;
		}

		boolean success = LiveEffectEngine.setEffectOn(true);
		if (success) {
			statusText.setText(R.string.status_playing);
			toggleEffectButton.setText(R.string.stop_effect);
			isPlaying = true;
			EnableUI(false);
		} else {
			statusText.setText(R.string.status_open_failed);
			isPlaying = false;
		}
	}

	private void stopEffect() {
		Log.d(TAG, "Playing, attempting to stop");
		LiveEffectEngine.setEffectOn(false);
		resetStatusView();
		toggleEffectButton.setText(R.string.start_effect);
		isPlaying = false;
		EnableUI(true);
	}

	private boolean isRecordPermissionGranted() {
		return (ActivityCompat.checkSelfPermission(this, Manifest.permission.RECORD_AUDIO) ==
				PackageManager.PERMISSION_GRANTED);
	}

	private void requestRecordPermission(){
		ActivityCompat.requestPermissions(
				this,
				new String[]{Manifest.permission.RECORD_AUDIO},
				AUDIO_EFFECT_REQUEST);
	}

	private void resetStatusView() {
		statusText.setText(R.string.status_warning);
	}

	@Override
	public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions,
										   @NonNull int[] grantResults) {

		if (AUDIO_EFFECT_REQUEST != requestCode) {
			super.onRequestPermissionsResult(requestCode, permissions, grantResults);
			return;
		}

		if (grantResults.length != 1 ||
				grantResults[0] != PackageManager.PERMISSION_GRANTED) {

			// User denied the permission, without this we cannot record audio
			// Show a toast and update the status accordingly
			statusText.setText(R.string.status_record_audio_denied);
			Toast.makeText(getApplicationContext(),
					getString(R.string.need_record_audio_permission),
					Toast.LENGTH_SHORT)
					.show();
		} else {
			// Permission was granted, start live effect
			toggleEffect();
		}
	}
}