import {ApiService} from '../services/api.service'

const QueryService = {
   getReadingSet: async function() {
        let res = await ApiService.get(`/data`)
        return res
    },
    saveReadingSet: async function(data){
        let qstring = `/save_reading_set`
        return await ApiService.post(qstring, data)
    }
}

export {QueryService}