import Vue from 'vue';
import Buefy from 'buefy';
import App from './App.vue';
import router from './router';
import store from './store';

import '../custom-styles.scss';

import {ApiService} from "./services/api.service"
import {TokenService} from "./services/storage.service"
import VueNativeSock from 'vue-native-websocket'

import { library } from '@fortawesome/fontawesome-svg-core'
import { fas } from '@fortawesome/free-solid-svg-icons'
import { FontAwesomeIcon } from '@fortawesome/vue-fontawesome'

library.add(fas)
Vue.component('fa-icon', FontAwesomeIcon)

Vue.config.productionTip = false;

Vue.use(Buefy, {defaultIconComponent: 'fa-icon',
defaultIconPack: 'fas',});

Vue.use(VueNativeSock, 'ws://jamestev.myddns.me:801', {reconnection: true, connectManually:true})
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

