<template>
  <div>
    <b-field v-if="ports.length > 0" label="serial ports" label-position="on-border">
      <b-select placeholder="Select port" v-model="selectedPort">
        <option v-for="(p, idx) in ports" :value="p" :key="idx">
          {{ p }}
        </option>
      </b-select>
    </b-field>
  </div>
</template>

<script>
import { SerialService } from "@/services/serial.service";

export default {
  name: "SerialHandler",
  props: {},
  data() {
    return {
      ports: [],
      selectedPort: null
    };
  },
  mounted(){
    // this.getPorts()
  },
  methods: {
    getPorts() {
      SerialService.getAvailablePorts().then(
        (ports) => {
          alert(ports)
          this.ports = ports;
        },
        (err) => {
          console.log(err);
        }
      );
    },
  },
};
</script>
