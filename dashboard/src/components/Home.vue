<template>
  <div>
    <div class="container" style="padding-bottom: 50px">
      <section class="hero is-fullheight has-text-left">
        <div>
          <div class="container">
            <!-- <img src="/assets/invictus-logo.svg" width="256px"> -->

            <div class="columns" style="padding-top: 50px">
              <div class="column is-6">
                <h1 class="title is-2" style="margin-bottom: 5px">Spectre</h1>
                <h1 class="heading">Spectrum Analyser</h1>
              </div>
              <div class="column has-text-right" v-if="selectedDevice" style="margin-top: 5px">
                <p class="title is-4">{{ selectedDevice.owner }}</p>
                <p class="subtitle is-6">
                  device ID: <b-tag> {{ selectedDevice.id }}</b-tag>
                </p>
              </div>
            </div>

            <b-tabs v-model="viewTab" style="padding-top: 0px" position="is-right" type="is-toggle">
              <b-tab-item label="Setup">
                <div id="infoCapture" class="is-left" style="padding: 0px 0px 20px 0px">
                  <div class="columns" style="padding-top: 30px">
                    <div class="column is-8">
                      <p class="title is-5">Sample information</p>
                      <p class="subtitle is-6" style="margin-bottom: 20px">
                        Information pertaining to the sample and the owner of the spectrometer
                      </p>
                      <b-field label="device owner" label-position="on-border">
                        <b-select placeholder="Select device owner" v-model="selectedDevice">
                          <option v-for="(d, idx) in devices" :value="d" :key="idx">
                            {{ d.owner }}
                          </option>
                        </b-select>
                      </b-field>
                      <b-field style="margin-top: 30px" label="sample name" label-position="on-border">
                        <b-input v-model="sampleName"></b-input>
                      </b-field>

                      <div class="columns" style="margin-top: 15px">
                        <div class="column is-8">
                          <b-field label="concentration %" label-position="on-border">
                            <b-input :max="100" :min="0" type="number" v-model="concentration"></b-input>
                          </b-field>
                        </div>
                        <div class="column has-text-right">
                          <p class="title" style="margin-top: 0px">
                            <b-tag class="is-medium" v-if="concentration < 100" type="is-danger">adulterated</b-tag>
                            <b-tag class="is-medium" v-else type="is-success">genuine</b-tag>
                          </p>
                        </div>
                      </div>

                      <div class="columns" style="margin-top: 15px">
                        <div class="column is-8">
                          <b-field label="API (active ingredient)" label-position="on-border">
                            <b-autocomplete v-model="apiType" :data="filteredAPIs" placeholder="e.g. paracetamol" clearable @select="option => (apiType = option)">
                              <template #empty>No matches - add your own?</template>
                            </b-autocomplete>
                          </b-field>
                        </div>
                        <div class="column has-text-right">
                          <b-field label="API Mass [mg]" label-position="on-border">
                            <b-input :min="0" type="number" v-model="apiMass"></b-input>
                          </b-field>
                        </div>
                      </div>

                      <div class="columns" style="margin-top: 15px">
                        <div class="column is-8">
                          <b-field label="adulterant" label-position="on-border">
                            <b-autocomplete v-model="adulterantType" :data="filteredAdulterants" placeholder="e.g. white sugar" clearable @select="option => (adulterantType = option)">
                              <template #empty>No matches - add your own?</template>
                            </b-autocomplete>
                          </b-field>
                        </div>
                        <div class="column has-text-right">
                          <b-field label="Adulterant Mass [mg]" label-position="on-border">
                            <b-input :min="0" type="number" v-model="adulterantMass"></b-input>
                          </b-field>
                        </div>
                      </div>

                      <div class="columns" style="margin-top: 15px">
                        <div class="column is-8">
                          <b-field label="solvent" label-position="on-border">
                            <b-autocomplete v-model="solvent" :data="filteredSolvents" placeholder="e.g. distilled water" clearable @select="option => (solvent = option)">
                              <template #empty>No matches - add your own?</template>
                            </b-autocomplete>
                          </b-field>
                        </div>
                        <div class="column has-text-right">
                          <b-field label="solvent volume [ml]" label-position="on-border">
                            <b-input :min="0" type="number" v-model="solventVol"></b-input>
                          </b-field>
                        </div>
                      </div>

                      <b-field style="margin-top: 30px" label="sample description" label-position="on-border">
                        <b-input v-model="sampleDescr" type="textarea" placeholder="Other relevant details such as API manufacturer, adulterant and/or solvent details"></b-input>
                      </b-field>
                    </div>
                    <div class="column">
                      <p class="title is-5">On-device Parameters</p>
                      <p class="subtitle is-6" style="margin-bottom: 20px">
                        Pertinent parameters from the onboard spectrometer controller
                      </p>
                      <b-field grouped>
                        <div v-for="[param, val] of Object.entries(config.deviceParamDefaults)" :key="param">
                          <b-field :label="param" label-position="on-border" expanded>
                            <b-input :value="val" type="number"></b-input>
                          </b-field>
                        </div>
                      </b-field>
                    </div>
                  </div>
                </div>

                <serial-handler></serial-handler>
              </b-tab-item>

              <b-tab-item label="Live view" :disabled="!requiredInfoCaptured">
                <div>
                  <div>
                    <div class="columns">
                      <div class="column is-8">
                        <p class="title is-4">Live Spectrum</p>
                        <p v-if="dataLoading" class="subtitle has-text-info is-6">
                          Awaiting data...
                        </p>
                        <p v-else class="subtitle is-6">
                          Showing spectrum for sample:
                          <b-tag>{{ sampleName }}</b-tag>
                        </p>
                      </div>
                      
                    </div>

                    <Plotly :data="chartData.data" :layout="chartData.layout" style="margin-top: 0px; padding: 0px"></Plotly>
                  </div>
                  <div class="columns" style="padding-top: 30px">
                    <div class="column is-2">
                      <b-field label="readings per set" label-position="on-border">
                        <b-input :value="readingSetLen" v-model="readingSetLen" type="number" :min="1" :max="10"></b-input>
                      </b-field>
                    </div>
                    <div class="column is-2">
                      <b-field label="reading period (sec)" label-position="on-border">
                        <b-input type="number" :value="readingPeriodSec" v-model="readingPeriodSec" :min="0.1" :max="4.0" :step="0.1"></b-input>
                      </b-field>
                    </div>
                    <div class="column">
                        <p class="title is-6" style="margin-top:10px">last calibration: 
                          <b-tag v-if="lastCal" class="is-info">{{lastCal.toLocaleString()}}</b-tag>
                          <b-tag v-else class="is-danger">never</b-tag>
                        </p>
                      </div>

                    <div class="column has-text-right buttons">
                      <b-button class="button is-primary has-text-right" :disabled="dataLoading" :loading="isRecordingCal" @click="recordReadingSet(true)">Record calibration set</b-button>
                      <b-button class="button is-primary has-text-right" :disabled="dataLoading" :loading="isRecording" @click="recordReadingSet(false)">Record set </b-button>
                    </div>
                  </div>

                  <b-progress size="is-medium" v-if="isRecording" type="is-primary" :value="recProgress" show-value></b-progress>
                </div>
              </b-tab-item>
              <b-tab-item label="Sample view" :disabled="readingSet.length == 0 || isRecording || isRecordingCal">
                <div v-if="readingSet && readingSet.length > 0">
                  <div class="columns">
                    <div class="column is-5">
                      <h2 class="title is-3">Reading Set</h2>
                      <p class="subtitle is-6">
                        showing {{ readingSetLen }} readings for sample <b-tag>{{ sampleName }}</b-tag> (measured every {{ readingPeriodSec }} seconds)
                      </p>
                    </div>
                    <div class="column" style="margin-top: 15px">
                      <b-tag v-if="!readingSetSaved" type="is-info" size="is-medium">not saved</b-tag>
                      <b-tag v-else size="is-medium" type="is-success"> saved</b-tag>
                    </div>
                  </div>

                  <Plotly :data="readingSetChartData.data" :layout="readingSetChartData.layout" style="margin-top: 0px; padding: 0px"></Plotly>

                  <div class="has-text-right">
                    <b-button class="button is-primary" :loading="apiLoading" :disabled="apiLoading || readingSetSaved" @click="saveReadingSet">Save data </b-button>
                  </div>
                </div>
              </b-tab-item>
            </b-tabs>
          </div>
        </div>
      </section>
    </div>
  </div>
</template>

<script>
// @ is an alias to /src
import { mapGetters, mapActions } from "vuex";
import { Plotly } from "vue-plotly";
import { QueryService } from "@/services/query.service";
import config from "@/config/config.json";

import SerialHandler from "@/components/SerialHandler.vue";

export default {
  name: "home",
  components: {
    Plotly,
    SerialHandler
  },
  data() {
    return {
      config,
      concentration: 100,
      counterfeit: false,
      dilution: true,
      sensorData: [],
      readingSet: [],
      adulterantType: "white sugar",
      adulterantMass: 0,
      apiType: "paracetamol",
      apiMass: 0,
      solventVol: 0,
      solvent: "distilled water (room temp)",
      sampleName: "test sample",
      sampleDescr: "",
      devices: [],
      viewTab: 0,
      lastCal: null,
      deviceParams: null,
      selectedDevice: null,
      readingSetLen: 5,
      readingSetSaved: false,
      readingPeriodSec: 0.5,
      isRecording: false,
      isRecordingCal: false,
      validConcentration: false, // whether or not concentration value matches API/adult. ratio
      recProgress: 0.0,
      apiLoading: false,
      apis: ["paracetamol", "ibuprofen", "viagra", "oxalic acid", "L-glutamine"],
      adulterants: ["white sugar", "table salt", "maltodextrin", "castor sugar"],
      solvents: ["distilled water (room temp)", "distilled water (boiling)", "acetone", "saline", "alcohol 70%", "alcohol 75%", "alcohol 80%"],
      calibrationSet: []
    };
  },
  beforeMount() {
    // this.$connect(); //connect ws
    this.$options.sockets.onmessage = msg => {
      this.handleMsg(msg.data);
    };
    this.$options.sockets.onopen = data => {};
  },
  mounted() {
    // populate params with config defaults
    this.readingSetLen = this.config.samplingDefaults.readingsPerSet;
    this.readingPeriodSec = this.config.samplingDefaults.readingPeriodSec;
    this.devices = this.config.devices;
    this.deviceParams = this.config.deviceParamDefaults;
  },
  computed: {
    dataLoading() {
      return !this.sensorData || this.sensorData.length == 0;
    },
    requiredInfoCaptured() {
      let check1 = this.selectedDevice && this.sampleName.length > 0;
      let check2 = this.adulterantType.length * this.apiType.length * this.solvent.length > 0;
      return check1 && check2;
    },
    filteredSolvents() {
      return this.filterOptions(this.solvents, this.solvent);
    },
    filteredAdulterants() {
      return this.filterOptions(this.adulterants, this.adulterantType);
    },
    filteredAPIs() {
      return this.filterOptions(this.apis, this.apiType);
    },
    chartData(data, title = "") {
      var x = [];
      var y = this.sensorData.map(yi => (yi * 100) / 255);
      var colors = [];
      for (var i = 0; i < this.sensorData.length; i++) {
        x[i] = 400 + i * 2.5;
        colors[i] = "hsl(" + 250 * (1 - i / 128) + ", 100%, 80%)";
      }
      let chartData = [
        {
          x: x.reverse(),
          y: y,
          type: "bar",
          autoscale: false,
          name: "long zig",
          marker: {
            color: colors.reverse(),
            line: { color: "rgba(85, 95, 128, 1)", width: 0.5 }
          }
        }
      ];
      let layout = {
        title: title,
        bargap: 0.02,
        margin: { t: 20, r: 0 },
        font: { size: 12, family: "Avenir" },
        xaxis: { title: "Wavelength (nm)" },
        yaxis: { title: "Intensity %", range: [0, 100], autoscale: false }
      };

      return { data: chartData, layout: layout };
    },
    readingSetChartData(data, title = "") {
      let rSet = this.readingSet;
      let x = rSet[0].values.map((el, i) => 400 + 400 * (i / rSet[0].values.length)); // create wavelength range

      let chartData = rSet.map((r, i) => {
        return {
          y: r.values,
          x: x,
          type: "line",
          name: "sample " + (i + 1),
          marker: {
            line: { color: "rgba(85, 95, 128, 1)", width: 0.5 }
          }
        };
      });

      let layout = {
        title: title,
        bargap: 0.02,
        margin: { t: 20, r: 0 },
        font: { size: 12, family: "Avenir" },
        xaxis: { title: "Wavelength (nm)" },
        yaxis: { title: "Intensity %", range: [0, 100], autoscale: false }
      };

      return { data: chartData, layout: layout };
    },
    ...mapGetters("auth", ["loggedIn", "authenticating", "authenticationError", "authenticationErrorCode"])
  },
  methods: {
    ...mapActions("auth", ["login"]),
    filterOptions(source, query) {
      return source.filter(option => {
        return (
          option
            .toString()
            .toLowerCase()
            .indexOf(query.toLowerCase()) >= 0
        );
      });
    },
    async recordReadingSet(calibration) {
      this.recProgress = 0;
      this.readingSetSaved = false;
      this.readingSet = [];

      let nReadings = this.readingSetLen;
      let readPeriod = this.readingPeriodSec;

      if (calibration) {
        nReadings = 3;
        readPeriod = 1.0;
        this.isRecordingCal = true;
      } else {
        this.isRecording = true;
      }

      for (let i = 0; i < nReadings; i++) {
        this.readingSet.push({
          timestamp: Date.now(),
          values: this.sensorData.map(yi => (yi * 100) / 255)
        });
        await sleep(readPeriod * 1000);
        this.recProgress = ((i + 1) * 100) / nReadings;
      }
      this.isRecording = false;
      if (!calibration) {
        this.viewTab = 2;
        this.isRecording = false;
      } else {
        this.calibrationSet = this.readingSet; // update calibration set
        this.readingSet = []; // clear reading set
        this.isRecordingCal = false;
        this.lastCal = new Date(Date.now())
      }
    },
    trigger() {
      this.$socket.send("operate gate");
    },
    saveReadingSet() {
      this.apiLoading = true;
      let payload = this.compileData();
      QueryService.saveReadingSet(payload)
        .then(res => {
          if (Object.keys(res.data).length == 0) {
            this.errorMessage("Couldn't save reading set :(");
            return;
          }
          this.readingSetSaved = true;
          this.successMessage("Reading set saved successfully");
        })
        .catch(res => {
          this.errorMessage("Couldn't save reading set - check console for details");
          console.log(res)
        })
        .finally(() => (this.apiLoading = false));
    },
    errorMessage(msg) {
      this.$buefy.toast.open({
        duration: 2500,
        message: msg,
        position: "is-top",
        type: "is-danger"
      });
    },
    successMessage(msg) {
      this.$buefy.toast.open({
        duration: 2500,
        message: msg,
        position: "is-bottom-right",
        type: "is-success"
      });
    },
    compileData() {
      let data = {
        device_id: this.selectedDevice.id,
        timestamp: Date.now(),
        readings: this.readingSet,
        calibration_readings: this.calibrationSet,
        sample_name: this.sampleName,
        sample_descr: this.sampleDescr,
        api_type: this.apiType,
        api_mass: this.apiMass,
        adulterant_type: this.adulterantType,
        adulterant_mass: this.adulterantMass,
        solvent: this.solvent,
        solvent_vol: this.solventVol,
        concentration: this.concentration,
        params: this.deviceParams ? this.deviceParams : {}
      };
      return data;
    },
    checkSampleNumbers() {
      let valid = true;
      if (this.adulterantMass == 0) {
        // pure sample
        valid &= this.concentration == 100;
      } else if (this.apiMass == 0) {
        // completely adulterated
        valid &= this.concentration == 0;
      } else if (Math.abs((this.apiMass * 100) / this.adulterantMass - this.concentration) > 5) {
        // more than 5% deviation
        console.log("Excess deviation detected");
        valid = false;
      }
      return valid;
    },
    handleMsg(data) {
      if (data instanceof Blob) {
        let fileReader = new FileReader();
        fileReader.readAsArrayBuffer(data);
        let _this = this;
        fileReader.onload = function(event) {
          let dataView = new DataView(fileReader.result);
          let out = new Array(dataView.byteLength);
          for (let i = 0; i < dataView.byteLength; i++) {
            out[i] = dataView.getUint8(i);
          }
          _this.sensorData = out;
        };
      } else if (data instanceof String || typeof data === "string") {
        if (data.startsWith("__params__")) {
          this.deviceParams = JSON.parse(data.substring(10));
          this.$buefy.toast.open({
            duration: 2500,
            message: "Device parameters received",
            position: "is-bottom-right",
            type: "is-info"
          });
        }
      }
    }
  }
};

function sleep(ms) {
  return new Promise(resolve => setTimeout(resolve, ms));
}
</script>

<style scoped>
#main {
  background: url("/assets/graphic.svg") no-repeat right;
}
</style>
