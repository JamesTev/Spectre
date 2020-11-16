<template>
  <div>
    <div
      :class="status == 0 ? 'has-background-c3' : 'has-background-primary'"
      style="height: 3px"
    ></div>
    <div class="container">
      <form v-if="showLogin">
        <div class="columns" style="padding-top: 10px">
          <div class="column is-4 is-offset-8">
            <form>
              <b-field
                class="has-text-left"
                label="Username"
                :type="{ 'is-danger': loginError }"
              >
                <b-input
                  minlength="1"
                  :disabled="authenticating"
                  v-model="username"
                ></b-input>
              </b-field>

              <b-field
                label="Password"
                class="has-text-left"
                :type="{ 'is-danger': authenticationErrorCode == 401 }"
                :message="{
                  'Incorrect login credentials': authenticationErrorCode == 401,
                }"
              >
                <b-input
                  :disabled="authenticating"
                  type="password"
                  v-model="password"
                  password-reveal
                >
                </b-input>
              </b-field>

              <b-field grouped group-multiline>
                <b-button
                  :loading="authenticating"
                  :disabled="authenticating"
                  class="button field is-primary"
                  @click="handleLogin()"
                >
                  <span>submit</span>
                </b-button>
                <b-button
                  class="button field is-light"
                  @click="showLogin = false"
                >
                  <span>hide</span>
                </b-button>
              </b-field>
            </form>
          </div>
        </div>
      </form>
      <section class="hero is-fullheight has-text-left">
        <div class="hero-body">
          <div class="container">
            <!-- <img src="/assets/invictus-logo.svg" width="256px"> -->
            <h1 class="title" style="margin-bottom:5px">Spectre</h1>
            <h1 class="heading">Spectrum Analyser</h1>
            <h2 class="subtitle" style="padding-top: 20px">Spectrum for sample x</h2>
            <!-- <h2 class="subtitle">Access portal</h2>
            <div class="field has-addons">
              <p class="control">
                <input
                  v-model="pw"
                  class="input is-c3"
                  type="password"
                  placeholder="password"
                />
              </p>
              <p class="control">
                <b-button class="button field is-c3" @click="trigger"
                  >submit</b-button
                >
              </p>
            </div> -->

            <Plotly
              :data="chartData.data"
              :layout="chartData.layout"
              style="margin-top: 0px; padding: 0px"
            ></Plotly>
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

export default {
  name: "home",
  components: {
    Plotly
  },
  data() {
    return {
      showLogin: false,
      username: "",
      password: "",
      loginError: false,
      pw: "",
      status: 0, //closed
    };
  },
  beforeMount() {
    // this.$connect(); //connect ws
    // this.$options.sockets.onmessage = (msg) => {
    //   this.handleMsg(msg.data);
    // };
    // this.$options.sockets.onopen = (data) => {};
  },
  mounted() {},
  computed: {
    chartData(data, title = "") {
      var x = [];
      var y = []
      var colors = []
      for (var i = 0; i < 128; i ++) {
        y[i] = Math.random();
        x[i] = 400+i*2.5
        colors[i] = "hsl("+i*2+", 100%, 80%)"
      }
      let chartData = [
        {
          x: x,
          y: y,
          type: "bar",
          name: "long zig",
          marker: {
            color: colors,
            line: { color: "rgba(85, 95, 128, 1)", width: 0.5 }
          }
        }
      ];
      let layout = {
        title: title,
        bargap: 0.02,
        margin: { t: 20, r: 0 },
        font: { size: 12, family: "Avenir" },
        xaxis: { title: "Wavelength (μm)" },
        yaxis: { title: "Intensity" }
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
      if (data.includes("P0")) {
        this.status = data.endsWith("0") ? 0 : 1;
      }
    },
    handleLogin() {
      // Perform a simple validation that email and password have been typed in
      if (this.username != "" && this.password != "") {
        this.login({ username: this.username, password: this.password }).then(
          (loggedIn) => {
            if (!loggedIn) {
              if (this.authenticationErrorCode != 401) {
                this.$buefy.toast.open({
                  duration: 2000,
                  message: "Unable to reach the login server :(",
                  position: "is-top",
                  type: "is-danger",
                });
              }
            }
          }
        );
        this.password = "";
      }
    },
  },
};
</script>

<style scoped>
#main {
  background: url("/assets/graphic.svg") no-repeat right;
}
</style>
