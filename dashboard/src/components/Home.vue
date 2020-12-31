<template>
  <div>
    <div class="container" style="padding-bottom: 50px">
      <section class="hero is-fullheight has-text-left">
        <div>
          <div class="container">
            <!-- <img src="/assets/invictus-logo.svg" width="256px"> -->
            <h1
              class="title is-2"
              style="padding-top: 40px; margin-bottom: 5px"
            >
              Spectre
            </h1>
            <h1 class="heading">Spectrum Analyser</h1>

            <div
              id="infoCapture"
              class="is-left"
              style="padding: 20px 0px 20px 0px"
            >
              <div class="columns" style="padding-top: 30px">
                <div class="column is-6">
                  <p class="title is-5" style="padding-top: 10px">
                    Owner information
                  </p>
                  <p class="subtitle is-6" style="margin-bottom: 20px">
                    Information to record the device owner
                  </p>
                  <b-field label="Device Owner" label-position="on-border">
                    <b-select
                      placeholder="Select device owner"
                      v-model="selectedDevice"
                    >
                      <option v-for="(d, idx) in devices" :value="d" :key="idx">
                        {{ d.owner }}
                      </option>
                    </b-select>
                  </b-field>
                </div>
                <div class="column">
                  <p class="title is-5" style="padding-top: 10px">
                    On-device Parameters
                  </p>
                  <p class="subtitle is-6" style="margin-bottom: 20px">
                    Pertinent parameters from the onboard spectrometer
                    controller
                  </p>
                  <b-field grouped>
                    <div
                      v-for="[param, val] of Object.entries(
                        config.deviceParamDefaults
                      )"
                      :key="param"
                    >
                      <b-field
                        :label="param"
                        label-position="on-border"
                        expanded
                      >
                        <b-input :value="val" type="number"></b-input>
                      </b-field>
                    </div>
                  </b-field>
                </div>
              </div>
            </div>

            <b-tabs v-model="viewTab" style="padding-top:0px" position="is-right" type="is-toggle" v-if="!dataLoading && requiredInfoCaptured">
              <b-tab-item label="Live view">
                <div>
              <div>
                <h2 class="title is-4" style="padding-top: 20px">
                  Sample Spectrum
                </h2>
                <Plotly
                  :data="chartData.data"
                  :layout="chartData.layout"
                  style="margin-top: 0px; padding: 0px"
                ></Plotly>
              </div>
              <div class="columns" style="padding-top: 30px">
                <div class="column is-3">
                  <b-field label="Number of readings per set">
                    <b-slider
                      :min="1"
                      :max="10"
                      ticks
                      class="is-primary"
                      v-model="readingSetLen"
                    ></b-slider>
                  </b-field>
                </div>
                <div class="column is-3">
                  <b-field label="Sample period (sec)">
                    <b-slider
                      :min="0.1"
                      :max="4.0"
                      :step="0.1"
                      class="is-primary"
                      v-model="readingPeriodSec"
                    ></b-slider>
                  </b-field>
                </div>
                <div class="column has-text-right">
                  <b-button
                    class="button is-primary has-text-right"
                    :loading="isRecording"
                    @click="recordReadingSet"
                    >Record set
                  </b-button>
        
                </div>
              </div>

              <b-progress
                size="is-medium"
                v-if="isRecording"
                type="is-primary"
                :value="recProgress"
                show-value
              ></b-progress>
            </div>
            <div v-if="dataLoading" style="padding-top: 40px">
              <p class="subtitle is-5">awaiting data...</p>
            </div>

              </b-tab-item>
              <b-tab-item label="Sample view" :disabled="readingSet.length == 0">
                <div v-if="readingSet && readingSet.length > 0">

                   <h2 class="title is-5" style="padding-top: 20px">
                  Sample Spectrum
                </h2>
                <Plotly
                  :data="readingSetChartData.data"
                  :layout="readingSetChartData.layout"
                  style="margin-top: 0px; padding: 0px"
                ></Plotly>
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
import config from "@/config/config.json";

export default {
  name: "home",
  components: {
    Plotly,
  },
  data() {
    return {
      config,
      sensorData: [],
      readingSet: [],
      devices: [],
      viewTab: 0,
      selectedDevice: null,
      readingSetLen: 5,
      readingPeriodSec: 0.5,
      isRecording: false,
      recProgress: 0.0,
    };
  },
  beforeMount() {
    this.$connect(); //connect ws
    this.$options.sockets.onmessage = (msg) => {
      this.handleMsg(msg.data);
    };
    this.$options.sockets.onopen = (data) => {};
  },
  mounted() {
    // populate params with config defaults
    this.readingSetLen = this.config.samplingDefaults.readingsPerSet;
    this.readingPeriodSec = this.config.samplingDefaults.readingPeriodSec;
    this.devices = this.config.devices;
  },
  computed: {
    dataLoading() {
      return !this.sensorData || this.sensorData.length == 0;
    },
    requiredInfoCaptured() {
      return this.selectedDevice;
    },
    chartData(data, title = "") {
      var x = [];
      var y = this.sensorData.map((yi) => (yi * 100) / 255);
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
            line: { color: "rgba(85, 95, 128, 1)", width: 0.5 },
          },
        },
      ];
      let layout = {
        title: title,
        bargap: 0.02,
        margin: { t: 20, r: 0 },
        font: { size: 12, family: "Avenir" },
        xaxis: { title: "Wavelength (nm)" },
        yaxis: { title: "Intensity %", range: [0, 100], autoscale: false },
      };

      return { data: chartData, layout: layout };
    },
    readingSetChartData(data, title = "") {
      let rSet = this.readingSet;
      let x = rSet[0].map((el, i) => 400 + 400 * (i / rSet[0].length)); // create wavelength range

      let chartData = rSet.map((r, i) => {
        return {
          y: r,
          x: x,
          type: "line",
          name: "sample " + (i + 1),
          marker: {
            line: { color: "rgba(85, 95, 128, 1)", width: 0.5 },
          },
        };
      });

      let layout = {
        title: title,
        bargap: 0.02,
        margin: { t: 20, r: 0 },
        font: { size: 12, family: "Avenir" },
        xaxis: { title: "Wavelength (nm)" },
        yaxis: { title: "Intensity %", range: [0, 100], autoscale: false },
      };

      return { data: chartData, layout: layout };
    },
    ...mapGetters("auth", [
      "loggedIn",
      "authenticating",
      "authenticationError",
      "authenticationErrorCode",
    ]),
  },
  methods: {
    ...mapActions("auth", ["login"]),
    async recordReadingSet() {
      this.recProgress = 0;
      this.isRecording = true;
      this.readingSet = [];
      for (let i = 0; i < this.readingSetLen; i++) {
        this.readingSet.push(this.sensorData.map((yi) => (yi * 100) / 255));
        await sleep(this.readingPeriodSec * 1000);
        this.recProgress = ((i + 1) * 100) / this.readingSetLen;
      }
      console.log(this.readingSet);
      this.isRecording = false;
      this.viewTab = 1
    },
    trigger() {
      if (this.pw == "zouzou") {
        this.$socket.send("operate gate");
        this.$buefy.toast.open({
          duration: 1000,
          message: `trigger sent`,
          position: "is-top-right",
          type: "is-success",
        });
      } else {
        this.$buefy.toast.open({
          duration: 1000,
          message: `wrong password`,
          position: "is-bottom",
          type: "is-danger",
        });
      }
    },
    handleMsg(data) {
      // let buffer = new Uint8Array(data).buffer;
      // let dataView = new DataView(buffer);
      // console.log(dataView)

      if (data instanceof Blob) {
        let fileReader = new FileReader();
        fileReader.readAsArrayBuffer(data);
        let _this = this;
        fileReader.onload = function (event) {
          let dataView = new DataView(fileReader.result);
          let out = new Array(dataView.byteLength);
          for (let i = 0; i < dataView.byteLength; i++) {
            out[i] = dataView.getUint8(i);
          }
          _this.sensorData = out;
        };
      }
    },
  },
};

function sleep(ms) {
  return new Promise((resolve) => setTimeout(resolve, ms));
}
</script>

<style scoped>
#main {
  background: url("/assets/graphic.svg") no-repeat right;
}
</style>
