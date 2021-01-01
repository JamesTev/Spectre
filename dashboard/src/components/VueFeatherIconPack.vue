<template>
  <component :is="iconComponent"/>
</template>

<script>
// all icon that you need
import { ChevronLeftIcon, ChevronRightIcon, CheckIcon} from "vue-feather-icons";

export default {
  components: {
    ChevronLeftIcon,
    ChevronRightIcon,
    CheckIcon
  },
  props: {
    icon: [String, Array],
    size: String,
    customClass: String
  },
  computed: {
    iconComponent() {
      return this.icon[1] + "Icon";
    }
  }
};
</script>