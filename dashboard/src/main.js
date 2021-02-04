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

Vue.config.productionTip = false;

Vue.use(Buefy);

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

