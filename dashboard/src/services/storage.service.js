import VueCookies from  'vue-cookies';

const TOKEN_KEY = 'access_token'
const REFRESH_TOKEN_KEY = 'refresh_token'

/**
 * Manage the how Access Tokens are being stored and retreived from storage.
 *
 * Current implementation stores to localStorage. Local Storage should always be
 * accessed through this instace.
**/

const TokenService = {
    getToken() {
        return VueCookies.get(TOKEN_KEY)
    },

    saveToken(accessToken) {
        VueCookies.set(TOKEN_KEY, accessToken, '12h')
    },

    removeToken() {
        VueCookies.remove(TOKEN_KEY)
    },

    getRefreshToken() {
        return VueCookies.get(REFRESH_TOKEN_KEY)
    },

    saveRefreshToken(refreshToken) {
        VueCookies.set(REFRESH_TOKEN_KEY, refreshToken, '1d')
    },

    removeRefreshToken() {
        VueCookies.remove(TOKEN_KEY)
    }

}

export { TokenService }