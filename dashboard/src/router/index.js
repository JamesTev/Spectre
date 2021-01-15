import Vue from 'vue';
import VueRouter from 'vue-router';
import Home from '../components/Home.vue';

import {TokenService} from '../services/storage.service'
import { ToastProgrammatic as toast } from 'buefy'

Vue.use(VueRouter);

const routes = [
  {
    path: '/',
    name: 'home',
    component: Home,
  },
  // {
  //   path: '/dashboard',
  //   name: 'dashboard',
  //   component: Dashboard,
  //   meta:{
  //     requiresAuth: true
  //   }
  // },
];

const router = new VueRouter({
  mode: 'history',
  base: process.env.BASE_URL,
  routes,
});

router.beforeEach((to, from, next) => {
  const loggedIn = !!TokenService.getToken()

  if(to.matched.some(record => record.meta.requiresAuth)){
    if(loggedIn){
      next()
    }else{
      toast.open({type: 'is-danger', message: "You'll need to login first."})
      next({
        name: 'home'
      })
    }
  }else{
    next()
  }
})
export default router;
