const SerialService = {
   getAvailablePorts: async function() {
        return SerialPort.list()
    },
    saveReadingSet: async function(data){
        let qstring = `/save_reading_set`
        return await ApiService.post(qstring, data)
    }
}

export {SerialService}