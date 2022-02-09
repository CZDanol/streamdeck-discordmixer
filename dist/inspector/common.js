// this is our global websocket, used to communicate from/to Stream Deck software
// and some info about our plugin, as sent by Stream Deck software
var websocket = null,
    uuid = null,
    actionInfo = {},
    settings = {},
    globalSettings = {};

function connectElgatoStreamDeckSocket(inPort, inUUID, inRegisterEvent, inInfo, inActionInfo) {
    uuid = inUUID;
    // please note: the incoming arguments are of type STRING, so
    // in case of the inActionInfo, we must parse it into JSON first
    actionInfo = JSON.parse(inActionInfo); // cache the info
    websocket = new WebSocket('ws://localhost:' + inPort);

    // if connection was established, the websocket sends
    // an 'onopen' event, where we need to register our PI
    websocket.onopen = function () {
        // register property inspector to Stream Deck
        websocket.send(JSON.stringify({
            event: inRegisterEvent,
            uuid: inUUID
        }));

        // Ask for global settings
        websocket.send(JSON.stringify(son = {
            event: "getGlobalSettings",
            context: uuid
        }));
    }

    websocket.onmessage = function (ev) {
        let evd = JSON.parse(ev.data);
        if (evd.event == "didReceiveGlobalSettings") {
            let s = evd.payload.settings;
            globalSettings = s;
            document.getElementById("client_secret").value = s.client_secret ?? null;
            document.getElementById("client_id").value = s.client_id ?? null;
        }
    }

    settings = actionInfo.payload.settings;
    fillData(settings);
}

function updateSettings(key, value) {
    settings[key] = value;

    if (!websocket)
        return;

    {
        const json = {
            "action": actionInfo['action'],
            "event": "setSettings",
            "context": uuid,
            "payload": settings
        };
        websocket.send(JSON.stringify(json));
    }
}

function updateGlobalSettings(key, value) {
    globalSettings[key] = value;

    if (!websocket)
        return;

    {
        const json = {
            "event": "setGlobalSettings",
            "context": uuid,
            "payload": globalSettings
        };
        websocket.send(JSON.stringify(json));
    }
}