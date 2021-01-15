import Vue from 'vue';
import Buefy from 'buefy';
import App from './App.vue';
import router from './router';
import store from './store';
import "buefy/dist/buefy.css";

import '../custom-styles.scss';

import {ApiService} from "./services/api.service"
import {TokenService} from "./services/storage.service"
import VueNativeSock from 'vue-native-websocket'

import VueFeatherIconPack from "./components/VueFeatherIconPack.vue";

Vue.component("vue-feather-icon-pack", VueFeatherIconPack);

Vue.config.productionTip = false;

Vue.use(Buefy, {
  defaultIconComponent: "vue-feather-icon-pack",
  defaultIconPack: 'ft',
  customIconPacks: {
    ft: {
      sizes: {
        default: "1.5x",
        "is-small": "1x",
        "is-medium": "2x",
        "is-large": "3x"
      },
      iconPrefix: "",
      internalIcons: {
        check: "checkmark",
        information: "info",
        "check-circle": "checkmark-circle",
        alert: "alert-triangle",
        "alert-circle": "alert-circle",
        "arrow-up": "arrow-up",
        "chevron-right": "chevron-right",
        "chevron-left": "chevron-left",
        "chevron-down": "chevron-down",
        eye: "eye",
        "eye-off": "eye-off",
        "menu-down": "arrow-down",
        "menu-up": "arrow-up"
      }
    }
  }
});

Vue.use(VueNativeSock, 'ws://spectreEsp32.local:802', {reconnection: true, connectManually:false})

Vue.use(require('vue-cookies'))
Vue.$cookies.config('1d')

const baseURI = process.env.VUE_APP_BACKEND_URL

ApiService.init(baseURI)
if(TokenService.getToken()){
  ApiService.setHeader()
}

new Vue({
  router,
  store,
  render: h => h(App),
}).$mount('#app');

