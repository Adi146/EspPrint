import {
  LitElement,
  html,
  css
} from "https://unpkg.com/lit-element@2.4.0/lit-element.js?module";

class TerminalCard extends LitElement {
  static get properties() {
    return {
      _hass: {},
      config: {},
      gcodes: {type: Array},
      history: {type: Array},
      historyIndex: {type: Number},
      autoscroll: {type: Boolean}
    };
  }

  constructor() {
    super();
    this.config = {};
    this.gcodes = [];
    this.history = [];
    this.historyIndex = Number.POSITIVE_INFINITY;
    this.autoscroll = true;
  }

  render() {
    return html`
      <ha-card header="Terminal">
        <div id="output">
          ${this.gcodes.map(gcode => {
            return html`
              <div>
                <span class="source">
                  ${gcode.source}:
                </span>
                <span class="gcode">
                  ${gcode.gcode} 
                </span>
              </div>
            `
          })}
        </div>
        <div id="input">
          <form autocomplete="off" @submit="${(e) => {
            e.preventDefault();
            this.sendGCode();          
          }}">
            <input type="text" id="input" @keydown="${(e) => {
              this.onKeyPress(e);
            }}"/>
            <input type="submit" value="Send"/>
          </form>
        </div>
      </ha-card>
    `;
  }

  updated() {
    if (this.autoscroll) {
      var output = this.shadowRoot.getElementById("output");
      output.scrollTop = output.scrollHeight;
    }
  }

  sendGCode() {
    var input = this.shadowRoot.getElementById("input");
    var value = input.value;
    
    if (value != "") {
      const [domain, service] = this.config.service.split('.', 2);
      this._hass.callService(domain, service, {
        gcode: value
      });
  
      this.history.push(input.value);
      this.historyIndex = Number.POSITIVE_INFINITY;
      input.value = '';
    }
  }

  onKeyPress(e) {
    if (e.key != "ArrowUp" && e.key != "ArrowDown"){
      return;
    }
    e.preventDefault();

    if (e.key == "ArrowUp"){
      if (this.historyIndex > this.history.length && this.history.length > 0) {
        this.historyIndex = this.history.length - 1;
      }
      else if (this.historyIndex > 0) {
        this.historyIndex--;
      }
    }
    else if(e.key == "ArrowDown"){
      this.historyIndex++;
    }

    var value = this.historyIndex < this.history.length ? this.history[this.historyIndex] : "";

    var input = this.shadowRoot.getElementById("input");
    input.value = value;
    input.selectionStart = value.length;
  }

  set hass(hass) {
    if (!this._hass) {
      this._hass = hass;

      this._hass.connection.subscribeEvents((event) => {
        var output = this.shadowRoot.getElementById("output");
        this.autoscroll = output.scrollTop >= (output.scrollHeight - output.offsetHeight - 10);

        this.gcodes = this.gcodes.concat(JSON.parse(event.data.gcodes));
      }, this.config.event);
    }

    this._hass = hass;
  }

  setConfig(config) {
    if (config.printer_name) {
      this.config.event = "esphome." + config.printer_name + "_gcode_event";
      this.config.service = "esphome.espprint_" + config.printer_name + "_send_gcode";
    }

    if (config.event) {
      this.config.event = config.event;
    }
    if (config.service) {
      this.config.service = config.service;
    }

    this.checkConfig();
  }

  checkConfig() {
    if (!this.config.event) {
      throw new Error("event undefined");
    }
    if (!this.config.service) {
      throw new Error("service undefined");
    }
  }

  static get styles() {
    return css`
      #output {
        overflow: auto;
        padding: 0 16px;
        height: 200px;
      }
      #input {
        margin: 10px 8px;
      }
      .source {
        font-weight: bold;
        margin-right: 10px;
      }
      input[type=text] {
        width: 79%;
        border: none;
        color: var(--primary-text-color);
        background-color: transparent;
        border-bottom: 1px solid var(--primary-text-color);
      }
      input[type=submit] {
        width: 15%;
        border: none;
        color: var(--primary-text-color);
        background-color: var(--disabled-text-color);
      }
      input[type=text]:focus {
        outline: none;
        border-bottom: 1px solid var(--primary-color);
      }
    `;
  }
}

customElements.define('terminal-card', TerminalCard);